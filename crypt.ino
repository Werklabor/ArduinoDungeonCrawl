/*
* Global State
*/

// Orientation

#define NORTH 0
#define WEST 1
#define SOUTH 2
#define EAST 3

int previousOrientation = -1;
int orientation = NORTH;

// Location

int currentLocation[] = {0, 0};
int previousLocation[] = {0, 0};

// «Things» on/off the map

#define WALL -1
#define EMPTY 0
#define SPECIAL 1

// Map

#define WIDTH 8
#define LENGTH 8
int _map[WIDTH][LENGTH] = {
	{EMPTY, EMPTY, EMPTY, EMPTY,   EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, EMPTY,   EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, EMPTY,   EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, EMPTY,   EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, SPECIAL, EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, EMPTY,   EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, EMPTY,   EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, EMPTY,   EMPTY, EMPTY, EMPTY, EMPTY}
};

// Error

#define NO_ERROR -1
int error = NO_ERROR;


/*
* Button Interaction
*/

int normalizeButtonOrder[] = {15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0};
int currentButtonPressed = -1;
int getButtonPressed(){
	int button = IBridge_Read_Key();
	if(button == currentButtonPressed){
		return -1;
	}else if(button == 0){
		currentButtonPressed = -1;
		return -1;
	}else{
		currentButtonPressed = button;
		return normalizeButtonOrder[button - 1];
	}
}


/*
* Display Interaction
*/

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
* Visuals
*
* Available characters:
* ! " # $ % & ' ( ) * + , - . / : ; < = > ? @ [ ] _ ^
*/

void renderWelcomeScreen(){
	displayWriteString("              ");
	displayWriteString(" MysteryCrypt ");
	displayWriteString(" by @cdn      ");
	displayWriteString("              ");
	displayWriteString(" Press any    ");
	displayWriteString(" key to start ");
}

void renderMessage(char* firstRow, char* secondRow){ // Both with exactly 8 characters
	displayClear();
	displayWriteString(" ____________ ");
	displayWriteString(" I          I ");
	displayWriteString(" I ");
	displayWriteString(firstRow);
	displayWriteString(" I ");
	displayWriteString(" I ");
	displayWriteString(secondRow);
	displayWriteString(" I ");
	displayWriteString(" I__________I ");
	displayWriteString("              ");
}
void renderRoomWithContent(char* firstRow, char* secondRow){
	displayClear();
	displayWriteString(" %          / ");
	displayWriteString("   +------+   ");
	displayWriteString("   I");
	displayWriteString(firstRow);
	displayWriteString("I   ");
	displayWriteString("   I");
	displayWriteString(secondRow);
	displayWriteString("I   ");
	displayWriteString("   +------+   ");

	char x = currentLocation[0] + '0';
	char y = currentLocation[1] + '0';
	displayWriteString(" /   ");
	displayWriteChar(x);
	displayWriteString("  ");
	displayWriteChar(y);
	displayWriteString("   % ");
}

void renderWall(){
	renderRoomWithContent("//////", "//////");
}
void renderEmpty(){
	renderRoomWithContent("      ", "      ");
}
void renderSpecial(){
	renderRoomWithContent("  ?   ", "   @  ");
}

void renderError(){
	displayClear();
	switch(error){
		case 0:
		renderMessage("  Wall  ", " ahead! ");
		break;
		default:
		renderMessage(" ERROR! ", " ------ ");
	}

}


/*
* Orientation
*/

void setOrientation(int diff){
	orientation = orientation + diff;
	if(orientation > 3){
		orientation = 0;
	}else if(orientation < 0){
		orientation = 3;
	}
}

void setOrientationFromButton(int button){
	switch(button){
		case 4:
		setOrientation(1);
		break;
		case 7:
		setOrientation(-1);
		break;
	}
}
boolean detectAndRememberOrientationChange(){
	if(previousOrientation != orientation){
		previousOrientation = orientation;
		return true;
	}
	return false;
}


/*
* Location
*/

int getThingAtLocation(int x, int y){
	if(x < 0 || y < 0 || x >= WIDTH || y >= LENGTH){
		return WALL;
	}else{
		return _map[x][y];
	}
}

int xForOrientation(int orientation){
	switch(orientation){
		case NORTH:
		return currentLocation[0];
		case WEST:
		return currentLocation[0] + 1;
		case SOUTH:
		return currentLocation[0];
		case EAST:
		return currentLocation[0] - 1;
	}
}
int yForOrientation(int orientation){
	switch(orientation){
		case NORTH:
		return currentLocation[1] - 1;
		case WEST:
		return currentLocation[1];
		case SOUTH:
		return currentLocation[1] + 1;
		case EAST:
		return currentLocation[1];
	}
}

int whatAmILookingAt(int orientation){
	int x = xForOrientation(orientation);
	int y = yForOrientation(orientation);
	return getThingAtLocation(x, y);
}
boolean moveInDirection(int orientation){
	int x = xForOrientation(orientation);
	int y = yForOrientation(orientation);
	if(getThingAtLocation(x, y) == WALL){
		return false;
	}else{
		currentLocation[0] = x;
		currentLocation[1] = y;
		return true;
	}
}

void render() {
	int thing = whatAmILookingAt(orientation);
	switch(thing){
		case WALL:
		renderWall();
		break;
		case EMPTY:
		renderEmpty();
		break;
		case SPECIAL:
		renderSpecial();
		break;
		// default:
		// Log error…?
	}
}

void setLocationFromButton(int button){
	switch(button){
		case 1:
		case 2:
		if(!moveInDirection(orientation)){
			displayError(0);
		}
		break;
		// default:
		// Error logging
	}
}
boolean detectAndRememberLocationChange(){
	if(previousLocation[0] == currentLocation[0] && previousLocation[1] == currentLocation[1]){
		return false;
	}else{
		previousLocation[0] = currentLocation[0];
		previousLocation[1] = currentLocation[1];
		return true;
	}
}

boolean handleButton(int button){
	boolean rerender = false;

	setOrientationFromButton(button);
	rerender = rerender || detectAndRememberOrientationChange();

	setLocationFromButton(button);
	rerender = rerender || detectAndRememberLocationChange();

	return rerender;
}


/*
* Error Displaying
*/

void displayError(int errorCode){
	error = errorCode;
}
boolean showAndResetError(){
	if(error != NO_ERROR){
		renderError();
		delay(1000);
		error = NO_ERROR;
		return true;
	}
	return false;
}

/*
* Main Logic
*/

void setup(){
	displayInitialize();
}

void loop()
{
	if(handleButton(getButtonPressed()) || showAndResetError()){
		render();
	}
}
