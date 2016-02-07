


/*************************************************************
 * 
 * Read NFC Tag and get Temperature Data
 * Use FIFO file to communicate with python
 * script which sends termperature data to 
 * Android App using bluetooth.
 * 
 */

/***************************************************************
 * 
 * TODO: 
 *       - encapsulate reading the responses in the readData()
 *       
 *       - enclose debugging statements in conditionals and
 *         set up debugging levels DEBUG = 1, 2, 3...
 *         
 *       - finish going through the last few functions to
 *         clean them up and debug with tag.
 *         
 *       - Brush up the loop() and its use of states and response
 *         to exceptions.
 *         
 *       - Document the functions and code
 *       
 *       - Define constants (maybe) for the hardware instructions
 *       
 */




// CR95HF COMMANDS

// Provides Information about CR95HF
const byte CMD_GET_ID =     0x01;  // Get information
const byte CMD_SET_PROTO =  0x02;  // Selects rf comm protocol
const byte CMD_TXRX =       0x04;  // Transmit / Recive
const byte CMD_IDLE =       0x07;  // Put in idle state
const byte CMD_READ_REG =   0x08;  // Reads CR95HF registers
const byte CMD_WRITE_REG =  0x09;  // Writes CR95HF registers
const byte CMD_SET_BAUD =   0x0A;  // Sets the UART baud rate
const byte CMD_ECHO =       0x55;  // CR95HF resonds with echo

// CR95HF CONSTANTS
const byte PROTO_15693 =    0x01;


byte TXBuffer[100];     // transmit buffer
byte RXBuffer[100];     // receive buffer
char NFC_UID[8];       // Will be used to Store UID
int LEDPIN = 13;
byte calibrationData[4];  // Calibration Data


// Serial State variable
byte responseCode;
int responseLength;
 



/*
 * Get pointers to the serial objects
 * to use to pass to function calls
 * will be useful for two readers
 */

HardwareSerial* sd = &Serial;
HardwareSerial* s1 = &Serial1;
HardwareSerial* s2 = &Serial2;



/**********************************************
 *  readData(HardwareSerial* s, int size, byte* buff)
 *  
 *  Read data from serial line into receive
 *  buffer.
 *  
 *  PARAMS:
 *  serial   - pointer to the serial object
 *  
 */
int readResponse(HardwareSerial* s) {

int i = 0;
  
 while(s->available()) {
    
    RXBuffer[i] = s->read();
    i++;
    
  }

  // Change Global State Vars
  responseLength = i;
  responseCode = RXBuffer[0];
  
  return i;

}

void sendCommand(HardwareSerial* s, byte* cmdVector) {

  int packetSize = (sizeof(cmdVector)/sizeof(cmdVector[0]));
  
  for(int i = 0; i < packetSize; i++) {

    s->write(cmdVector[i]);
    
  }
  
}


/**************************************************************
 *  Initialization Procedure
 *    This is an Arduino function that is called once at start
 *************************************************************/
void setup() {
  
  delay(10);                      // send a wake up    


  // Setup the LED 7 - J19 Pin 6
  pinMode(LEDPIN, OUTPUT);
  
  // Set up Arduino Virutal Serial
  Serial.begin(57600);
  Serial.println("what up");  
  
  // Set up Serial Communcation with Reader
  Serial1.begin(57600);
  Serial1.write((byte)0x00);   //Actual Wake-Up Call

  // WAIT
  delay(10);
}                                  

/*********************************************************************************
/////////////////////////// Echo Command //////////////////////////////////////
// This Tests to make sure that the BM019 is connected properly to the Edison
**********************************************************************************/

/**************************************
 * 
 * Check to see if we get an echo back
 * from the reader. That is if the UART
 * communication with the chip is working.
 * 
 * RETURNS 0 - fail, 1 - sucess
 * 
 */
int sendEcho() {
  Serial.println("\n\n**********ECHO***********\n");
  Serial1.write(CMD_ECHO);  // Echo command


  // Command packet
  byte command[] = {CMD_ECHO};
  //sendCommand(s1, command);
  //readResponse(s1);

  
  // Waiting for Response
  delay(500);
  
  int i = 0;

  while(Serial1.available()) {
    
    RXBuffer[i] = Serial1.read();
    i++;
    
  }

  responseLength = i;
  responseCode = RXBuffer[0];

  Serial.print("Length of echo response is: ");
  Serial.println(responseLength,DEC);

  Serial.println("Contents of the received response");
  for (byte i=0; i < responseLength; i++) {    
    Serial.print(RXBuffer[i], HEX);
    Serial.print(':');
  }
  Serial.println();
  
  // Receives an Extra 0x0 then receceives 
  // the echo command this is noted in the 
  // Data Sheet from Solutions Cubed on 
  // Page 4 in reference to another command.
 
  if ((responseCode == CMD_ECHO) ) { // Received echo back
 
    return 1;

    
  } else {

    Serial.println("Echo Failed...");
    
    return 0;
  }
} 

int getDeviceID() {

  Serial.println("\n\n**********GET ID***********\n");

  Serial1.write((byte)0x01);  // id command
  Serial1.write((byte)0x00);  // length of data that follows is: 2

  delay(100);

  int i = 0;

  while(Serial1.available()) {
    
    RXBuffer[i] = Serial1.read();
    i++;
    
  }

  responseLength = i;

  Serial.print("Length of resonse for request id: ");
  Serial.println(i, DEC);

  // 1 - Response Code
  // 2 - Length of response
  // 
  
  int j = 0;
  char id[100];
  
  Serial.println("Contents of get id response");
  for (int i=0; i < responseLength; i++){     
    Serial.print(RXBuffer[i],HEX);

    if(i > 1 && i < (responseLength - 2)) {
      id[j] = RXBuffer[i];
      j++;
    }
      
    Serial.print(":");
  }
  Serial.println();

  Serial.println(id);
    
}


 
////////////////////////Set Protocol Command///////////////////////////////////
// This tells the CR95HF Chip what protocol it will use.

 int setProtocol() {
  Serial.println("\n\n**********PROTO***********\n");

  /*
  Serial1.write(CMD_SET_PROTO);  // protocol command
  Serial1.write((byte)0x02);  // length of data that follows is: 2
  Serial1.write(PROTO_15693);  // code for ISO/IEC 15693
  Serial1.write(0b00001001);  // Parameters byte 
  */
  Serial1.write((byte)0x02);  // protocol command
  Serial1.write((byte)0x02);  // length of data that follows is: 2
  Serial1.write((byte)0x01);  // code for ISO/IEC 15693
  Serial1.write((byte)0x09);  // Parameters

  // Response malformed if delay(10)...
  delay(50);

  int i = 0;

  while(Serial1.available()) {
    
    RXBuffer[i] = Serial1.read();
    i++;
    
  }

  responseLength = i;
    
  Serial.print("Length of resonse for protocol set: ");
  Serial.println(i, DEC);

  // 1 - Response Code
  // 2 - Length of response
  
  Serial.println("Contents of Protocol Set response");
  for (int i=0; i < responseLength; i++){     
    Serial.print(RXBuffer[i],HEX);
    Serial.print(":");
  }
  Serial.println();

  // Reader will respond with 0x00 0x00 if protocol set successfully
  if ((RXBuffer[0] == 0x0) & (RXBuffer[1] == 0x0)) {
    
    Serial.println("Protocol Set Command OK");
    
    return 1;

  } else {
    
    Serial.println("Protocol Set Command FAIL - Invalid Response"); 
    
    return 0;
  }
}

 

//////////////////////// Inventory Command ///////////////////////////////////
// This command Polls all NFCs in range an asks for their UID
 
int doInventory() {


// Inventory Flags
// b0 - Subcarrier      0:Single      1:Double
// b1 - Data Rate       0:Low         1:High
// b2 - Inventory       0:No          1:Yes
// b3 - Prortocol Ext   0:Always 0    1:RFU
// b4 - AFI             0:Always 0    1:Not Allowed
// b5 - # of Slots      0:16 Slots    1:1 Slot
// b6 - Option          0:Always 0    1:RFU
// b7 - RFU             0:Always 0    1:RFU
  
  //// step 1 send the command 
  Serial1.write((byte)0x04);        // Send Receive CR95HF command
  Serial1.write((byte)0x03);        // length of data that follows is 3
  Serial1.write((byte)0b00100110);  // request Flags byte 
  Serial1.write((byte)0x01);        // Inventory Command for ISO/IEC 15693
  Serial1.write((byte)0x00);        // mask length for inventory command 
  //[Mask Length Not Used. Has something to do with Addressed mode]//

  delay(50);
  
  Serial.println("\n\n********** INVENTORY ***********\n");
   
  int i = 0;

  while(Serial1.available()) {
    
    RXBuffer[i] = Serial1.read();
    i++;
    
  }

  responseLength = i;
  responseCode = RXBuffer[0];

  // DEBUG


  Serial.print("Length of response is :");
  Serial.println(responseLength,DEC);
  
  Serial.println("Packet Dump:");
  // TODO: dumpBuffer(byte*, int)
  for(int i = 0; i < responseLength; i++) {

    Serial.print(RXBuffer[i], HEX);
    Serial.print(":");     
    
  }
  Serial.println();


  

  if (responseCode == 0x80) { // is response code good?
    
    // Store UID to NFC_UID array
    for (int i=0, j=4; i < 8; i++, j++){ // Debugging Portion
      NFC_UID[i] = RXBuffer[j];  // data 
    } 

    unsigned long uid = RXBuffer[4]<<24 | RXBuffer[5]<<16 | RXBuffer[6]<<8 | RXBuffer[7];
     
    // Print UID to Serial
    Serial.print("UID: ");
    Serial.println(uid,HEX);
    
    return 1;

    
  } else {
    
    return 0;
      
   }
}


//////////////////////// Buffer Command ///////////////////////////////////
// FLAGS
// b0 - Subcarrier      0:Single      1:Double
// b1 - Data Rate       0:Low         1:High
// b2 - Inventory       0:No          1:Yes
// b3 - Prortocol Ext   0:Always 0    1:RFU
// b4 - Select          0:All Tags    1:Selected Tag
// b5 - Address         0:Unaddressed 1:Addressed
// b6 - Option          0:Always 0    1:RFU
// b7 - RFU             0:Always 0    1:RFU

//Temperature Conversion
//The calibration data does not have to be included in the
//temperature conversion equation. The temperature value is
//calculated as:
//T(ºC) = code*0.169 – 92.7 – 5.4
//LSB = 0.169ºC
//offset = -92.7ºC
//offset calibration = 0.169 * 32 = 5.4ºC


/*********************
 * Read ARC_B register
 */


int regCommand(byte cmd[], int len) {

  Serial1.write(cmd,len);
  
  delay(50);
  
  int i = 0;

  while(Serial1.available()) {
    
    RXBuffer[i] = Serial1.read();
    i++;
    
  }

  responseLength = i;
  responseCode = RXBuffer[0]<<8 | RXBuffer[1];

  
  
  // DEBUG


  Serial.print("Length of response is :");
  Serial.println(responseLength,DEC);

  Serial.print("Response Code :");
  Serial.println(responseCode,HEX);
  
  Serial.println("Packet Dump:");
  // TODO: dumpBuffer(byte*, int)
  for(int i = 0; i < responseLength; i++) {

    Serial.print(RXBuffer[i], HEX);
    Serial.print(":");     
    
  }
 
  Serial.println();

  return 0;
  

}

/******************************************************
 * Write to the reader
 * This reads / writes Analog Register Configuration B
 * We use it to read and set the gain value for the
 * antenna
 */

int writeARCBGain(byte value) {

  byte writeIdx[] = {0x09,0x03,0x68,0x00,0x01};
  byte readReg[]= {0x08,0x03,0x69,0x01,0x00};
  
  // The gain is ARCB default D3
  // Change it to D0 where D is modulation
  // index and 3 = 27dB / 0 = 34dB
  
  byte writeARCB[] = {0x09,0x04,0x68,0x01,0x01,value};

  
  regCommand(writeIdx,5);
  regCommand(readReg,5);
  
  int result = regCommand(writeARCB,6);

  regCommand(writeIdx,5);
  regCommand(readReg,5);


  return result;
  

}

int readBlock(byte addr) {

//  byte request[] = {0x04,0x03,0x02,0x20,0x05};
//  
//  Serial.write(request,5);



  delay(500);
  //Serial1.available()){
  Serial1.write((byte)0x04);  // Send Receive CR95HF command
  Serial1.write((byte)0x03);  // length of data that follows is (if UID: 14 = E)
  Serial1.write((byte)0x02);  // request Flags byte
  Serial1.write((byte)0x20);  // Send Internal Calibration Command ISO 15693
  Serial1.write((byte)0x0E);  // Sends the Internal Calibration Data
  
    
  delay(2000);
  
  Serial.println("*************** READ BLOCK *************");
  
  int i = 0;

  while(Serial1.available()) {
    
    RXBuffer[i] = Serial1.read();
    i++;
    
  }

  responseLength = i;
  responseCode = RXBuffer[0];

  
  // DEBUG


  Serial.print("Length of response is :");
  Serial.println(responseLength,DEC);
  
  Serial.println("Packet Dump:");
  // TODO: dumpBuffer(byte*, int)
  for(int i = 0; i < responseLength; i++) {

    Serial.print(RXBuffer[i], HEX);
    Serial.print(":");     
    
  }
  Serial.println();


  if (responseCode == 0x80) {  // is response code good?
  
    Serial.println("Memory Bock Read");

    return 1;
  
  } else {
    
    Serial.println("Reading Block Failed.."); 
    return 0;
    
  }
  
}


/*******************************************************************************
//////////////////////// Get Calibration Data  /////////////////////////////////
*******************************************************************************/
int getCalibration() {
  
  //byte Calibration[4];      // Calibration
  //byte Cal_Return_Full[4];  // Dummy Calibration for Debugging

  /******************************
  * Step 1: Get Calibration Data 
  *******************************/
  Serial1.write((byte)0x04);  // Send Receive CR95HF command
  Serial1.write((byte)0x02);  // length of data that follows is 2
  Serial1.write((byte)0x02);  // request Flags byte
  Serial1.write((byte)0xA9);  // Get Calibration Command ISO 15693
  
  /*for (byte i=8; i>0; i--){   
    Serial1.write((byte) NFC_UID[i-1]);  // Sends the UID
  }*/
  
  delay(10);
  
  Serial.println("*************** GET INTERNAL CALIBRATION *************");
  
  int i = 0;

  while(Serial1.available()) {
    
    RXBuffer[i] = Serial1.read();
    i++;
    
  }

  responseLength = i;
  responseCode = RXBuffer[0];

  
  // DEBUG


  Serial.print("Length of response is :");
  Serial.println(responseLength,DEC);
  
  Serial.println("Packet Dump:");
  // TODO: dumpBuffer(byte*, int)
  for(int i = 0; i < responseLength; i++) {

    Serial.print(RXBuffer[i], HEX);
    Serial.print(":");     
    
  }
  Serial.println();

  /******************************************
  * Step 2: Store Internal Calibration Data
  *******************************************/

  if (responseCode == 0x80) {  // is response code good?
  
    Serial.println("Calibration Recieved OKAY response");
  
    // Store Internal Calibration to Calibration[] array    
    for (byte i=0, j=3; i < 4; i++, j++) {    
      calibrationData[i]=RXBuffer[j];         // Internal Calibration Data at bytes 4-7 
      Serial.print(calibrationData[i],HEX);
      Serial.print(":");
    } 
    Serial.println();

    long calibration = RXBuffer[4]<<24 | RXBuffer[5]<<16 | RXBuffer[6]<<8 | RXBuffer[7 ];

    Serial.println(calibration,HEX);

    return 1;
  
  } else {
    
    Serial.println("Getting Calibration Failed..."); 
    return 0;
    
  }
}




    
//// Step 3: Send Internal Calibration Data 

int setCalibration() {    

  //Build a packet

  delay(500);
  //Serial1.available()){
  Serial1.write((byte)0x04);  // Send Receive CR95HF command
  Serial1.write((byte)0x06);  // length of data that follows is (if UID: 14 = E)
  Serial1.write((byte)0x02);  // request Flags byte
  Serial1.write((byte)0xA5);  // Send Internal Calibration Command ISO 15693
  byte cal[] = {0x00, 0x00, 0xDE,0x00};
  //Serial1.write(cal, 4);  // Sends the Internal Calibration Data
  //Serial1.write(calibrationData, 4);  // Sends the Internal Calibration Data
  
  delay(2000);
  
  Serial.println("***************** SET INTERNAL CALIBRATION ***************");

  int i = 0;

  while(Serial1.available()) {
    
    RXBuffer[i] = Serial1.read();
    i++;
    
  }

  responseLength = i;
  responseCode = RXBuffer[0];

  //DEBUG
  Serial.println("Packet Dump:");
  // TODO: dumpBuffer(byte*, int)
  for(int i = 0; i < responseLength; i++) {

    Serial.print(RXBuffer[i], HEX);
    Serial.print(":");     
    
  }
  Serial.println();


  

  if (responseCode== 0x80) { // is response code good?
  
    Serial.println("Set Internal Calibration - Success");

    return 1;
  
  } else {

    Serial.println("Set Internal Calibration - Failure");
    return 0;
  }
}

//// Step 4:  Get Temperature
/*******************************************************************************
 * 
 * 
 */

int getTemp(byte* temp) {

  Serial1.write((byte)0x04);  // Send Receive CR95HF command
  Serial1.write((byte)0x02);  // length of data that follows is 10
  Serial1.write((byte)0x02);  // request Flags byte
  Serial1.write((byte)0xAD);  // Send Get Temperature Command ISO 15693
  
  //SEND UID
  /*for (byte i=0; i<8; i++){   
    Serial1.write((byte) NFC_UID[i]);  // Sends the UID
  }*/
  
  delay(50);
  
  Serial.println("***************** GET TEMPERATURE ***************");

  int i = 0;
  
  while(Serial1.available()) {
    
    RXBuffer[i] = Serial1.read();
    i++;
    
  }

  responseLength = i;
  responseCode = RXBuffer[0];


  // DEBUG
  Serial.println("Packet Dump:");
  // TODO: dumpBuffer(byte*, int)
  for(int i = 0; i < responseLength; i++) {

    Serial.print(RXBuffer[i], HEX);
    Serial.print(":");     
    
  }
  Serial.println();

  
  if (responseCode == 0x80) { // is response code good?
    
    Serial.println("Read Temperature - Success");
    // Do something

    // Since chip has a 10 bit ADC try to get only 10...
    
    int temps = ((0x33 & RXBuffer[4]) << 8) | RXBuffer[3];
    temp[0] = RXBuffer[3];
    temp[1] = RXBuffer[4];

    //Debug 
    //Serial.println(*temp, DEC);
    Serial.println(temps,DEC);

    double t = temps*0.169-92.7-5.4;
    t = (t*9/5) + 32;
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" F");
  
    return 1;
    
  } else {
    
    //Serial.println("Inventory Command FAIL");
    Serial.println("Read Temperature - Failure");
    return 0;
       
  } 
}


 /////////////////////// Reset to Ready Command /////////////////////////////////
 
void Reset_to_Ready()
{   
  Serial1.write((byte)0x04);  // Send Receive CR95HF command
  Serial1.write((byte)0x02);  // length of data that follows is 10
  Serial1.write((byte)0x02);  // request Flags byte
  Serial1.write((byte)0x26);  // Get Calibration Command ISO 15693
  
  if (Serial1.available()) {
    RXBuffer[0] = Serial1.read();  // response code
    RXBuffer[1] = Serial1.read();  // length of data
  }
   
  for (byte i=0;i<RXBuffer[1];i++) {  // Sets Condition for Writing Length   
    RXBuffer[i+2]=Serial1.read();  // Writes Data to RXBuffer 
  }
  
  Serial.println(RXBuffer[0], DEC);    // Debugging Portion of Code
  Serial.println(RXBuffer[1], DEC);
  
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////// Setting up for Bluetooth ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


//////////////////////// Read Temperature Command ///////////////////////////////

//// REPLACE WITH THE ONE IN SALUT


void Save_Temperature() {
  
Serial.print("Temperature is: ");        // Debugging Portion of Code


      int temp;
      int count;
      FILE *to_python;
      
      for (byte i=0, j=5;i<2; i++, j--){   
        //Temperature[i]=RXBuffer[j];  // data 
        //Serial.print(Temperature[i], DEC);  // Debugging Portion of Code
        //Serial.print(" ");
      } 
      Serial.println();
      
      //String Byte_One = String(Temperature[0], BIN);
      //Serial.println(Temperature[0], BIN);  // Debugging Portion of Code
      //temp = atoi(Temperature);
      
      //fprintf(to_python, "%d%", Temperature[0]);      
      
      //fputs(Temperature, to_python);
      
      fflush(to_python);
      count = count+1;
      Serial.println("Temperature : ");
      //Serial.println(Temperature);
      Serial.println("temp : ");
      Serial.println(temp);
      Serial.println("Count : ");
      Serial.println(count);
      
}

///////////////////// Loop Code //////////////////////////////////////////
// This code Loops Indefinetly (Effectively the Brain of our System)

byte temp[2];
int state =  0;
int count = 0;
int DELAY = 1000;
int calRcvd = 0;

void loop() {

  delay(50);

  switch(state) {
  
    case 0:
  
      if(sendEcho() == 1) {
        state++;
        count--;
      } else {
        count++;
        break;;
      }


      delay(DELAY);

    case 1:
      
      if(setProtocol() == 1) {
        delay(100);
        getDeviceID();      

        // Change the gain to max
        //writeARCBGain(0xD0);
        
        
        state++;
        count--;
        digitalWrite(LEDPIN,LOW);
      } else {
        count++;
        break;;
      }

      delay(DELAY);
      
    case 2:

      delay(DELAY);
      
      if(doInventory() == 1) {
        state++;
        count--;
        digitalWrite(LEDPIN,HIGH);
        break;
      } else {
        count++;
        break;;
      }

    case 3:

      delay(DELAY);
      if(!calRcvd) {
      //if(1) {
        if(getCalibration()==1) {
          calRcvd = 1;

          //readBlock(0x05);

          
          state++;
          count--;
          //break;;
        } else {
          count++;
          state--;
          break;;
        }
      }

    case 4:
      delay(DELAY);
      if(setCalibration() == 1) {
      //if(1) {
        state++;
        count--;
      } else {
        count++;
        //state = 2;
        state--;
        break;;
      }

    case 5:
      delay(DELAY);
      if(getTemp(temp) == 1) {

        
      } else {
        count++;
        state = 2;
        break;;
      }

    default:

      break;
  
  }

  if(state > 2) {

    digitalWrite(LEDPIN,HIGH);

  } else {

    digitalWrite(LEDPIN,LOW);

  }

    
}
