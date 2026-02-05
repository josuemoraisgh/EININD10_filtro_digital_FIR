#pragma once
#include <Arduino.h>

// Header-only, focado no subconjunto AT mais compatível do ESP-01 do SimulIDE.
// Usa CIPSEND com length (sem modo transparente). Um cliente TCP (MUX=0).

class SimuESP01_TCP_Min {
public:
  explicit SimuESP01_TCP_Min(Stream& esp, Stream* dbg=nullptr,
                             unsigned long tmo=8000)
  : _esp(esp), _dbg(dbg), _tmo(tmo) {}

  void setDebug(Stream* dbg) { _dbg = dbg; }

  bool begin() { flushIn(); return at("AT", "OK"); }

  bool wifiJoin(const char* ssid, const char* pass) {
    // ATE0 e CWMODE=1 são quase universais; se algum falhar, seguimos adiante.
    at("ATE0", "OK");
    at("AT+CWMODE=1", "OK");
    char cmd[160];
    snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", ssid, pass);
    // Algumas builds respondem só "OK" em vez de "WIFI CONNECTED".
    if (!(sendCmdWait(cmd, "WIFI CONNECTED", 20000) || sendCmdWait(cmd, "OK", 20000)))
      return false;
    at("AT+CIFSR", "OK"); // opcional (IP)
    return true;
  }

  bool connectTCP(const char* host, uint16_t port) {
    at("AT+CIPMUX=0", "OK"); // single
    char cmd[160];
    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%u", host, port);
    // Algumas implementações retornam "CONNECT" e depois "OK"; aceitamos qualquer um.
    return (sendCmdWait(cmd, "OK") || sendCmdWait("AT+CIPSTATUS", "STATUS:", 2000));
  }

  bool close() { return at("AT+CIPCLOSE", "OK"); }

  // Envia payload com CIPSEND=len (modo mais compatível)
  size_t send(const uint8_t* data, size_t len) {
    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%u", (unsigned)len);
    if (!sendCmdWait(cmd, ">", _tmo)) return 0;
    _esp.write(data, len);
    // Aceitamos "SEND OK" ou "OK"
    if (!(waitFor("SEND OK", _tmo) || waitFor("OK", _tmo))) return 0;
    return len;
  }
  size_t print(const String& s) { return send((const uint8_t*)s.c_str(), s.length()); }
  size_t write(uint8_t b) { return send(&b, 1); }

  // Recepção (caso a outra ponta envie algo; parse simples de +IPD)
  void poll() { parseIPD(); }
  int available() const { return _avail; }
  int read() {
    if (_avail==0) return -1;
    int c = _fifo[_tail];
    _tail = (_tail+1) & (kFifoSize-1);
    _avail--;
    return c;
  }

private:
  Stream& _esp;
  Stream* _dbg;
  unsigned long _tmo;

  static const size_t kFifoSize = 256;
  uint8_t _fifo[kFifoSize]{};
  size_t _head=0,_tail=0,_avail=0;

  void push(uint8_t c){
    if (_avail<kFifoSize){ _fifo[_head]=c; _head=( _head+1 )&(kFifoSize-1); _avail++; }
    else { /* drop oldest */ _tail=( _tail+1 )&(kFifoSize-1); _fifo[_head]=c; _head=( _head+1 )&(kFifoSize-1); }
  }

  void flushIn(){ while(_esp.available()) _esp.read(); }

  bool waitFor(const char* tok, unsigned long tmo){
    unsigned long t0=millis(); size_t m=0;
    while (millis()-t0<tmo){
      while(_esp.available()){
        int c=_esp.read();
        if(_dbg) _dbg->write(c);
        // capturamos +IPD para RX
        _ipdFeed((char)c);
        if (tok[m]==(char)c){ if(++m==strlen(tok)) return true; }
        else m=(tok[0]==(char)c)?1:0;
      }
    }
    return false;
  }

  bool sendCmdWait(const char* cmd, const char* expect, unsigned long tmo=0){
    if(_dbg){ _dbg->print(F(">> ")); _dbg->println(cmd); }
    _esp.print(cmd); _esp.print("\r\n");
    return waitFor(expect, tmo? tmo:_tmo);
  }

  bool at(const char* cmd, const char* expect, unsigned long tmo=0){
    return sendCmdWait(cmd, expect, tmo);
  }

  // Parser mínimo de "+IPD,<len>:<data>"
  void _ipdFeed(char c){
    static enum {S_IDLE,S_P,S_I,S_D,S_LEN,S_DATA} st=S_IDLE;
    static size_t need=0, got=0;
    if (st==S_DATA){
      push((uint8_t)c);
      if(++got>=need) st=S_IDLE;
      return;
    }
    switch(st){
      case S_IDLE: st=(c=='+')?S_P:S_IDLE; break;
      case S_P: st=(c=='I')?S_I:S_IDLE; break;
      case S_I: st=(c=='P')?S_D:S_IDLE; break;
      case S_D: st=(c=='D')?S_LEN:S_IDLE; need=0; got=0; break;
      case S_LEN:
        if (c==':'){ st=S_DATA; }
        else if (c>='0' && c<='9'){ need = need*10+(c-'0'); }
        else { st=S_IDLE; }
        break;
      default: st=S_IDLE; break;
    }
  }

  void parseIPD(){
    while(_esp.available()){
      char c=_esp.read();
      _ipdFeed(c);
    }
  }
};