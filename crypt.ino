/*
 * Button Interaction */

int normalizeButtonOrder[] = {15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0};
int getButtonPressed(){
	int button = IBridge_Read_Key();
	if(button == 0){
		return false;
	}else{
		return normalizeButtonOrder[button - 1];
	}
}

/*
 * Display Interaction */

void displayInitialize(){
	IBridge_init();
  IBridge_LCD5110_clear();
}
void displayMoveCursor(int x, int y){
	IBridge_LCD5110_set_XY(x, y);
}
void displayClear(){
	IBridge_LCD5110_clear();
}
void displayReset(){
	IBridge_LCD5110_clear();
	IBridge_LCD5110_set_XY(0, 0);
}
void displayWriteChar(char character){
	IBridge_LCD5110_write_char(character);
}
void displayWriteString(char* text){
	IBridge_LCD5110_write_string(text);
}

/*
 * Visuals */

 void displayWelcomeScreen(){
 	displayWriteString("              ");
 	displayWriteString(" M E M O R Y  ");
 	displayWriteString(" -Zeilenwerk- ");
 	displayWriteString("              ");
 	displayWriteString(" Press a key  ");
 	displayWriteString(" to start!    ");
 }

// ! " # $ % & ' ( ) * + , - . / : ; < = > ? @ [ ] _ ^

void displayRoomWithContent(char* firstRow, char* secondRow){
	displayClear();
	//								 "--------------"
	displayWriteString(" %          / ");
	displayWriteString("   +------+   ");
	displayWriteString("<- I");
	  displayWriteString(firstRow);
		displayWriteString("I ->");
	displayWriteString("   I");
	  displayWriteString(secondRow);
		displayWriteString("I   ");
	displayWriteString("   +------+   ");
	displayWriteString(" /          % ");
}

void displayWall(){
	displayRoomWithContent("//////", "//////");
}
void displayDoor(){
	displayRoomWithContent("      ", "   @  ");
}

/*
 * Main Logic */

void setup(){
  displayInitialize();
}

int oldOrientation = -1;
int orientation = 0; // 0-3
void setOrientation(int diff){
	orientation = orientation + diff;
	if(orientation > 3){
		orientation = 0;
	}else if(orientation < 0){
		orientation = 3;
	}
}
boolean setOrientationFromButton(int button){
	switch(button){
		case 4:
			setOrientation(1);
			return true;
		case 7:
			setOrientation(-1);
			return true;
	}
	return false;
}

void loop()
{
  while(1){
    int button = getButtonPressed();
		boolean shouldPause = setOrientationFromButton(button);
		if(oldOrientation != orientation){
			oldOrientation = orientation;

			switch(orientation){
				case 0:
					displayWall();
					break;
				case 1:
					displayWall();
					break;
				case 2:
					displayDoor();
					break;
				case 3:
					displayWall();
					break;
			}
		}
		if(shouldPause) delay(500);
  }
}
