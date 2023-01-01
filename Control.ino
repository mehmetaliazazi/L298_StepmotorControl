volatile int dirData = 0;  // motor direction
int driveTable[]={B00101000,B00011000,B00010100,B00100100}; // sargılara gönderilen pulse değerleri
volatile int portDcopy = B00000000;  //D portundaki Son değer tutuldu
int DutyCounter;              // ISR registerinde gelen pulseTime kadar bekleterek istenilen uzunlukta pulse'ler üretildi
volatile int PulseTime;       //pulseTime*25us = OCR2A = 49 // pulse uzunluğunu belirlemek için kullanılmakta.
volatile int stepReg =0;      //gelen pulse'lere göre arttırıldı ve azaltıldı
volatile int stepPhase = 0;   //drivetable için indis olarak kullanıldı
volatile int turnnCntr = 0;   //Step motorun kaç adım atacağı burada belirlendi


void setup() {
  // put your setup code here, to run once:
     DDRD = 0b11111111;
     cli();//stop interrupts
     //set timer2 interrupt at 8kHz
     TCCR2A = 0;// set entire TCCR2A register to 0
     TCCR2B = 0;// same for TCCR2B
     TCNT2  = 0;//initialize counter value to 0
     // set compare match register for 8khz increments
     OCR2A = 49;// = (16*10^6) / (40000*8) - 1 (must be <256)//////// 25us
     // turn on CTC mode
     TCCR2A |= (1 << WGM21);
     // Set CS21 bit for 8 prescaler
     TCCR2B |= (1 << CS21);  
     // enable timer compare interrupt
     TIMSK2 |= (1 << OCIE2A);
     sei();//allow interrupts

      pinMode(10, OUTPUT);  //ENA ve ENB 10 nolu Pine bağlıdır.
      digitalWrite(10,HIGH);
   
     PulseTime = 80;
     for(int i=0;i<1;i++){  //Motor 1 defa 400 adım yani 2 devir sağa ardından 2 devir sola dönüp durdurulu
          dirData = 0;
          turnnCntr = 400;    //başlat 
          while(turnnCntr>0);  //bitmesini bekle
          delay(1000);
    
          dirData = 1;
          turnnCntr = 400;    //başlat 
          while(turnnCntr>0);  //bitmesini bekle
          delay(1000);
      }
          
      while(1){ //Motora bağlanan Ağırlık Motor durduktan sonra sabit kalıp motorun çarkını çevirmemesi için.
        digitalWrite(10,HIGH);
        delayMicroseconds(200);
        digitalWrite(10,LOW);
        delayMicroseconds(50);
      }
      
}


ISR(TIMER2_COMPA_vect){

   DutyCounter++; if(DutyCounter >= PulseTime) DutyCounter=0; //Duty counter gelen interruptları saymakta
                                                              // istenilen miktarda interrupt geldikten sonra aşağı satıra geçiyor
   if(turnnCntr>0){
  
      if(DutyCounter == 0){
  
           if(dirData == 0) stepReg++; else stepReg--;
        
           stepPhase = stepReg&3;  //StepReg son 2 basamağına bakılıyor
           portDcopy = PORTD & ~B00111100;  //IN1,IN2,IN3,IN4 sıfırlanıp temp değişkenine atanıyor
           PORTD = portDcopy | driveTable[stepPhase]; //Tablodan gereken pulse D portuna yollandı
           turnnCntr--;
       }
    }
 }




void loop() {
  // put your main code here, to run repeatedly:

}