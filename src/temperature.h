int DHpin = 4; // input/output pin
byte dat[5];

byte read_data()
{
    byte i = 0;
    byte result = 0;
    for (i = 0; i < 8; i++)
    {
        while (digitalRead(DHpin) == LOW)
            ;                  // wait 50us
        delayMicroseconds(30); // The duration of the high level is judged to determine whether the data is '0' or '1'
        if (digitalRead(DHpin) == HIGH)
            result |= (1 << (8 - i)); // High in the former, low in the post
        while (digitalRead(DHpin) == HIGH)
            ; // Data '1', waiting for the next bit of reception
    }
    return result;
}
void prepare_temp()
{
    digitalWrite(DHpin, LOW); // Pull down the bus to send the start signal
    delay(30);                // The delay is greater than 18 ms so that DHT 11 can detect the start signal
    digitalWrite(DHpin, HIGH);
    delayMicroseconds(40); // Wait for DHT11 to respond
    pinMode(DHpin, INPUT);
    while (digitalRead(DHpin) == HIGH)
        ;
    delayMicroseconds(80); // The DHT11 responds by pulling the bus low for 80us;

    if (digitalRead(DHpin) == LOW)
        delayMicroseconds(80);  // DHT11 pulled up after the bus 80us to start sending data;
    for (int i = 0; i < 5; i++) // Receiving temperature and humidity data, check bits are not considered;
        dat[i] = read_data();
    pinMode(DHpin, OUTPUT);
    digitalWrite(DHpin, HIGH); // After the completion of a release of data bus, waiting for the host to start the next signal
}

void initTemp()
{
    pinMode(DHpin, OUTPUT);
}

float tempInC()
{
    prepare_temp();
    float temp = dat[2] + dat[3] / 10.0; // Displays the integer bits of temperature;
    return temp;
}

float humidity()
{
    prepare_temp();
    float hum = dat[0] + dat[1] / 10.0; // Displays the integer bits of humidity;
    return hum;
}