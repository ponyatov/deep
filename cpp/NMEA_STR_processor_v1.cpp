const int dataBuferSize = 600;
char NMEAdataBufer[dataBuferSize];
const char NMEArecongGPS[8] = "$GPRMC,";
const char NMEArecongSD[8] = "$SDDPT,";
String STRtime;
String STRlat;
String STRlon;
String STRspeed;
String STRtrackAngle;
String STRdepth;
String STRtemperature;
String DMY;
char STRlat_route;
char STRlon_route;
char reliabilityGPSdata = 0;

int SDcomparisonInData = 0;
int GPScomparisonInData = 0;
int SDcomparisonErRecData = 0;
int GPScomparisonErRecData = 0;

int carriageDataGPS = 0;
int carriageDataSD = 0;
int statdata = 0;

void setup() {
	Serial.begin(9600);
	Serial1.begin(9600);
	Serial.setTimeout(10);
	Serial1.setTimeout(50);
}

byte GPSdataPreprocessor()  //***********************************************
{
	int GPSDataOK = 0;
	byte comparisonGPS = 0;

	GPScomparisonInData = Serial1.readBytes(NMEAdataBufer, 595);
	if (GPScomparisonInData > 60) {
		GPSDataOK = 3;
	}

	if (GPSDataOK == 3) {
		for (int recongi = 0; recongi < 8; recongi++) {
			if (NMEAdataBufer[recongi] == NMEArecongGPS[recongi])
				comparisonGPS++;

		}
		if (comparisonGPS == 7)
			GPSDataOK = 2;

		if (GPSDataOK == 2) {
			carriageDataGPS = 7;
			STRtime = readValString(carriageDataGPS, true);
			reliabilityGPSdata = (char) readValString(carriageDataGPS, true)[0];
			STRlat = readValString(carriageDataGPS, true);
			STRlat_route = (char) readValString(carriageDataGPS, true)[0];
			STRlon = readValString(carriageDataGPS, true);
			STRlon_route = (char) readValString(carriageDataGPS, true)[0];
			STRspeed = readValString(carriageDataGPS, true);
			STRtrackAngle = readValString(carriageDataGPS, true);
			DMY = readValString(carriageDataGPS, true);

			if (STRlat_route == 'N') {
				STRlat = '-' + STRlat;

			}

			if (STRlon_route == 'E') {

				STRlon = '-' + STRlon;

			}

			while (carriageDataGPS < 100) {
				if (NMEAdataBufer[carriageDataGPS] == '*') {
					GPSDataOK = 5;
					break;
				}
				carriageDataGPS++;
			}

			carriageDataGPS = 0;

		}

	}

	for (int cleari = 0; cleari < dataBuferSize; cleari++) {
		NMEAdataBufer[cleari] = ' ';

	}

	return GPSDataOK;
}

String readValString(int carriage, bool cariageflag) {

	String Databuffer;
	int countchar = 0;
	bool rot = true;

	while (rot) {
		char b = NMEAdataBufer[carriage];

		if ((('0' <= b) && (b <= '9')) || (b == '.')
				|| (('A' <= b) && (b <= 'Z'))) {

			Databuffer += b;
			carriage++;
			countchar++;
		}

		if ((b == ',') || (b == '*')) {

			rot = false;
			carriage++;
			countchar++;
		}

		if (countchar > 15) {
			if (cariageflag == true)
				GPScomparisonErRecData++;
			if (cariageflag == false)
				SDcomparisonErRecData++;
			break;
		}

	}

	if (cariageflag == true) {
		carriageDataGPS = carriage;
	}

	if (cariageflag == false) {
		carriageDataSD = carriage;
	}

	return Databuffer;
}

byte SDdataPreprocessor()    // **********************************************
{

	int SDDataOK = 3;
	byte comparisonSD = 0;
	int inDatabuf = 0;
	int whileDatReceiving = 0;
	int dcd = 0;
	SDcomparisonInData = Serial.readBytes(NMEAdataBufer, 50);
	if (GPScomparisonInData > 15) {
		SDDataOK = 3;
	}

	if (SDDataOK == 3) {
		for (int recongi = 0; recongi < 8; recongi++) {
			if (NMEAdataBufer[recongi] == NMEArecongSD[recongi])
				comparisonSD++;

		}
		if (comparisonSD == 7)
			SDDataOK = 2;

		if (SDDataOK == 2) {

			carriageDataSD = 7;
			STRdepth = readValString(carriageDataSD, false);
			STRtemperature = readValString(carriageDataSD, false);

			SDDataOK = 5;
			carriageDataSD = 0;

		}

	}

	for (int cleari = 0; cleari < dataBuferSize; cleari++) {
		NMEAdataBufer[cleari] = ' ';

	}

	return SDDataOK;

}

void loop() {

	if (Serial1.available()) {
		statdata = GPSdataPreprocessor();
		UART_data_Print_GPS();

	}
	if (Serial.available()) {
		statdata = SDdataPreprocessor();
		UART_data_Print_SD();

	}

	delay(2);
}

void UART_data_Print_GPS() {
	Serial.println("*A*");
	Serial.print("Time:");
	Serial.println(STRtime);
	Serial.print("Datatrue:");
	Serial.println(reliabilityGPSdata);
	Serial.print("Lat:");
	Serial.println(STRlat);
	Serial.print("Lon:");
	Serial.println(STRlon);
	Serial.print("Speed:");
	Serial.println(STRspeed);
	Serial.print("TrackAngle:");
	Serial.println(STRtrackAngle);
	Serial.print("Date:");
	Serial.println(DMY);

	Serial.print("DataCount:");
	Serial.println(GPScomparisonInData);
	Serial.print("DataErRec:");
	Serial.println(GPScomparisonErRecData);
	Serial.print("DataOK:");
	Serial.println(statdata);

	GPScomparisonErRecData = 0;
}

void UART_data_Print_SD() {

	Serial.println("*A*");
	Serial.print("Depth:");
	Serial.println(STRdepth);
	Serial.print("Temperature:");
	Serial.println(STRtemperature);

	Serial.print("DataCount:");
	Serial.println(SDcomparisonInData);
	Serial.print("DataErRec:");
	Serial.println(SDcomparisonErRecData);
	Serial.print("DataOK:");
	Serial.println(statdata);
	SDcomparisonErRecData = 0;
}
