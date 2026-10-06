#define MAJOR 0
#define MINOR 1
#define PATCH 0

#define VN__(a,b,c) #a "." #b "." #c
#define VN_(a,b,c) VN__(a,b,c)
#define VERSION_NUMBER VN_(MAJOR,MINOR,PATCH)

#define VERSION "5ed " VERSION_NUMBER

#define WINDOW_NAME VERSION
