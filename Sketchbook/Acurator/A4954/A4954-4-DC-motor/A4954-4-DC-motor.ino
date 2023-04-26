const int channel_a_enable  = 6;
const int channel_a_input_1 = 4;
const int channel_a_input_2 = 7;
const int channel_b_enable  = 5;
const int channel_b_input_3 = 3;
const int channel_b_input_4 = 2;

void setup()
{
  pinMode( channel_a_enable, OUTPUT );  // Channel A enable
  pinMode( channel_a_input_1, OUTPUT ); // Channel A input 1
  pinMode( channel_a_input_2, OUTPUT ); // Channel A input 2
  
  pinMode( channel_b_enable, OUTPUT );  // Channel B enable
  pinMode( channel_b_input_3, OUTPUT ); // Channel B input 3
  pinMode( channel_b_input_4, OUTPUT ); // Channel B input 4
  
  Serial.begin( 9600 );
  Serial.println("Starting up");
}

void loop()
{
      Serial.println("Channel A forward");
      analogWrite( channel_a_enable, 255);
      digitalWrite( channel_a_input_1, HIGH);
      digitalWrite( channel_a_input_2, LOW);
      delay(5000);
      allInputsOff();
    
      Serial.println("Channel A reverse");
      analogWrite( channel_a_enable, 255);
      digitalWrite( channel_a_input_1, LOW);
      digitalWrite( channel_a_input_2, HIGH);
      delay(5000);
      allInputsOff();
      
      Serial.println("Channel A forward half speed");
      analogWrite( channel_a_enable, 127);
      digitalWrite( channel_a_input_1, HIGH);
      digitalWrite( channel_a_input_2, LOW);
      delay(5000);
      allInputsOff();
}

void allInputsOff()
{
  digitalWrite( 4, LOW );
  digitalWrite( 7, LOW );
  digitalWrite( 6, LOW );
  digitalWrite( 3, LOW );
  digitalWrite( 2, LOW );
  digitalWrite( 5, LOW );
}
