#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#define strcasecmp _stricmp
#endif
#define USER_FILE      "users.txt"
#define VEHICLE_FILE   "vehicles.txt"
#define BOOKING_FILE   "bookings.txt"

#define MAX_NAME       50
#define MAX_PHONE      20
#define MAX_PASSWORD   33
#define MAX_ROLE       15
#define MAX_TYPE       20
#define MAX_DATE       20
#define MAX_PAYMENT    20
#define MAX_TRX        30
#define MAX_STATUS     20

#define MAX_SEATS      40
typedef struct
{
    int id;
    char username[MAX_NAME];
    char password[MAX_PASSWORD];
    char role[MAX_ROLE];
    char phone[MAX_PHONE];

}User;


typedef struct
{
    int id;
    int ownerId;

    char type[MAX_TYPE];
    char company[MAX_NAME];

    char from[MAX_NAME];
    char to[MAX_NAME];

    char departure[30];

    int totalSeats;
    int bookedSeats;

    float fare;

}Vehicle;


typedef struct
{
    int bookingId;
    int vehicleId;

    char username[MAX_NAME];

    char passenger[MAX_NAME];

    char phone[MAX_PHONE];

    char journeyDate[MAX_DATE];

    int seatNo;

    float amount;

    char paymentMethod[MAX_PAYMENT];

    char trxID[MAX_TRX];

    char status[MAX_STATUS];

}Booking;
char currentUser[MAX_NAME] = "";
char currentRole[MAX_ROLE] = "";

int currentUserId = 0;
typedef struct {
    uint32_t state[4];
    uint64_t bitCount;
    unsigned char buffer[64];
} MD5_CTX;

uint32_t K[64] = {
0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,
0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,
0x6b901122,0xfd987193,0xa679438e,0x49b40821,
0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,
0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,
0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,
0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,
0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,
0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,
0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391
};


int shift[64] = {
7,12,17,22,7,12,17,22,
7,12,17,22,7,12,17,22,

5,9,14,20,5,9,14,20,
5,9,14,20,5,9,14,20,

4,11,16,23,4,11,16,23,
4,11,16,23,4,11,16,23,

6,10,15,21,6,10,15,21,
6,10,15,21,6,10,15,21
};

/*=============================
            MD5 PART 1
=============================*/

uint32_t leftRotate(uint32_t x, uint32_t c)
{
    return (x << c) | (x >> (32 - c));
}


void md5Init(MD5_CTX *ctx)
{

    ctx->bitCount = 0;


    ctx->state[0] = 0x67452301;

    ctx->state[1] = 0xefcdab89;

    ctx->state[2] = 0x98badcfe;

    ctx->state[3] = 0x10325476;

}
/*=============================
            MD5 PART 2
        TRANSFORM FUNCTION
=============================*/

void md5Transform(MD5_CTX *ctx, unsigned char data[])
{
    uint32_t a,b,c,d,f,g,temp;

    uint32_t M[16];


    for(int i=0;i<16;i++)
    {
        M[i] =
        (uint32_t)data[i*4] |
        ((uint32_t)data[i*4+1] << 8) |
        ((uint32_t)data[i*4+2] << 16) |
        ((uint32_t)data[i*4+3] << 24);
    }



    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];



    for(int i=0;i<64;i++)
    {

        if(i < 16)
        {
            f = (b & c) | ((~b) & d);
            g = i;
        }

        else if(i < 32)
        {
            f = (d & b) | ((~d) & c);
            g = (5*i + 1) % 16;
        }

        else if(i < 48)
        {
            f = b ^ c ^ d;
            g = (3*i + 5) % 16;
        }

        else
        {
            f = c ^ (b | (~d));
            g = (7*i) % 16;
        }



        temp = d;

        d = c;

        c = b;



        b = b + leftRotate(
                a + f + K[i] + M[g],
                shift[i]
            );



        a = temp;

    }



    ctx->state[0] += a;

    ctx->state[1] += b;

    ctx->state[2] += c;

    ctx->state[3] += d;

}
 /*=============================
            MD5 PART 3
        UPDATE + FINAL
=============================*/


void md5Update(MD5_CTX *ctx,
               unsigned char data[],
               size_t len)
{

    size_t index;

    size_t partLen;

    size_t i = 0;



    index = (ctx->bitCount / 8) % 64;



    ctx->bitCount += len * 8;



    partLen = 64 - index;



    if(len >= partLen)
    {

        memcpy(&ctx->buffer[index],
               data,
               partLen);



        md5Transform(ctx,
                     ctx->buffer);



        for(i = partLen; i + 63 < len; i += 64)
        {
            md5Transform(ctx,
                         &data[i]);
        }



        index = 0;

    }



    memcpy(&ctx->buffer[index],
           &data[i],
           len - i);

}




/*=============================
            MD5 FINAL
=============================*/

void md5Final(unsigned char hash[],
              MD5_CTX *ctx)
{

    unsigned char padding[64] = {0x80};

    unsigned char bits[8];



    for(int i=0;i<8;i++)
    {
        bits[i] =
        (ctx->bitCount >> (8*i)) & 0xff;
    }



    size_t index = (ctx->bitCount / 8) % 64;



    size_t padLen;



    if(index < 56)
    {
        padLen = 56 - index;
    }

    else
    {
        padLen = 120 - index;
    }




    md5Update(ctx,
              padding,
              padLen);



    md5Update(ctx,
              bits,
              8);





    for(int i=0;i<4;i++)
    {

        hash[i] =
        (ctx->state[0] >> (8*i)) & 0xff;



        hash[i+4] =
        (ctx->state[1] >> (8*i)) & 0xff;



        hash[i+8] =
        (ctx->state[2] >> (8*i)) & 0xff;



        hash[i+12] =
        (ctx->state[3] >> (8*i)) & 0xff;

    }

}
/*=============================
            MD5 PART 4
            HASH FUNCTION
=============================*/

void md5Hash(char password[], char output[])
{

    MD5_CTX ctx;

    unsigned char digest[16];



    md5Init(&ctx);



    md5Update(&ctx,
              (unsigned char *)password,
              strlen(password));



    md5Final(digest,
             &ctx);




    for(int i=0;i<16;i++)
    {
        sprintf(&output[i*2],
                "%02x",
                digest[i]);
    }



    output[32]='\0';

}
void initAdmin()
{
    FILE *fp;
    User u;
    int found = 0;

    fp = fopen(USER_FILE, "r");

    if(fp != NULL)
    {
        while(fscanf(fp,
        "%d|%49[^|]|%32[^|]|%14[^|]|%19[^\n]",
        &u.id,
        u.username,
        u.password,
        u.role,
        u.phone) == 5)
        {
            if(strcmp(u.role,"admin")==0)
            {
                found = 1;
                break;
            }
        }

        fclose(fp);
    }


    if(found == 0)
    {
        char hash[33];

        md5Hash("admin123", hash);


        fp = fopen(USER_FILE, "a");

        if(fp == NULL)
        {
            printf("User File Error!\n");
            return;
        }


        fprintf(fp,
        "1|admin|%s|admin|01700000000\n",
        hash);


        fclose(fp);
    }
}
/*void refreshSystem()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    initAdmin();

    printf("\n====================================\n");
    printf("   SYSTEM REFRESH SUCCESSFUL\n");
    printf("====================================\n");
}*/
/*====================================================
                REGISTER USER
=====================================================*/
/*====================================================
                    REGISTRATION
=====================================================*/

void registerUser(const char *role)
{
    FILE *fp;

    User u, temp;
    char rawPassword[50];

    int maxId = 0;

    fp = fopen(USER_FILE, "a+");

    if(fp == NULL)
    {
        printf("\nUser File Open Error.\n");
        return;
    }

    rewind(fp);

    printf("\n=====================================\n");
    printf("          %s REGISTRATION\n", role);
    printf("=====================================\n");

    printf("Username : ");
    scanf(" %[^\n]", u.username);

    /* Check username already exists */
    while(fscanf(fp,
        "%d|%49[^|]|%32[^|]|%14[^|]|%19[^\n]",
        &temp.id,
        temp.username,
        temp.password,
        temp.role,
        temp.phone) == 5)
    {
        if(temp.id > maxId)
        {
            maxId = temp.id;
        }

        if(strcasecmp(temp.username, u.username) == 0)
        {
            printf("\nUsername Already Exists.\n");

            fclose(fp);
            return;
        }
    }

    u.id = maxId + 1;

    strcpy(u.role, role);

    printf("Phone Number : ");
    scanf(" %[^\n]", u.phone);

    printf("Password : ");
    scanf(" %[^\n]", rawPassword);

    /* Hash Password */
    md5Hash(rawPassword, u.password);

    /* Save User */
    fprintf(fp,
        "%d|%s|%s|%s|%s\n",
        u.id,
        u.username,
        u.password,
        u.role,
        u.phone
    );

    fclose(fp);

    printf("\n=====================================\n");
    printf("      Registration Successful!\n");
    printf("      User ID : %d\n", u.id);
    printf("      Username : %s\n", u.username);
    printf("      Role : %s\n", u.role);
    printf("=====================================\n");
}
/*====================================================
                    LOGIN
=====================================================*/

/*====================================================
                        LOGIN
=====================================================*/

int login(const char *role)
{
    FILE *fp;

    User u;

    char username[MAX_NAME];
    char password[50];
    char hash[33];

    fp = fopen(USER_FILE, "r");

    if(fp == NULL)
    {
        printf("\nUser File Not Found.\n");
        return 0;
    }

    printf("\n=====================================\n");
    printf("             %s LOGIN\n", role);
    printf("=====================================\n");

    printf("Username : ");
    scanf(" %[^\n]", username);

    printf("Password : ");
    scanf(" %[^\n]", password);

    /* Convert entered password to hash */
    md5Hash(password, hash);

    while(fscanf(fp,
        "%d|%49[^|]|%32[^|]|%14[^|]|%19[^\n]",
        &u.id,
        u.username,
        u.password,
        u.role,
        u.phone) == 5)
    {
        if(strcasecmp(username, u.username) == 0)
        {
            /* Check Role */
            if(strcmp(role, u.role) != 0)
            {
                printf("\nWrong Role Login.\n");

                fclose(fp);
                return 0;
            }

            /* Check Password */
            if(strcmp(hash, u.password) == 0)
            {
                strcpy(currentUser, u.username);
                strcpy(currentRole, u.role);

                currentUserId = u.id;

                fclose(fp);

                printf("\n=====================================\n");
                printf("        Login Successful!\n");
                printf("        Welcome %s\n", currentUser);
                printf("        Role : %s\n", currentRole);
                printf("=====================================\n");

                return 1;
            }
            else
            {
                printf("\nIncorrect Password.\n");

                fclose(fp);
                return 0;
            }
        }
    }

    fclose(fp);

    printf("\nUsername Not Found.\n");

    return 0;
}
/*====================================================
                    LOGOUT
=====================================================*/

void logout()
{
    if(strlen(currentUser)==0)
    {
        printf("\nNo User Logged In.\n");
        return;
    }

    printf("\n=====================================\n");
    printf(" User %s Logged Out Successfully.\n", currentUser);
    printf("=====================================\n");


    strcpy(currentUser,"");
    strcpy(currentRole,"");

    currentUserId = 0;
}
/*====================================================
                 VIEW PROFILE
=====================================================*/

void viewProfile()
{
    if(strlen(currentUser)==0)
    {
        printf("\nPlease Login First.\n");
        return;
    }


    FILE *fp;

    User u;

    fp=fopen(USER_FILE,"r");


    if(fp==NULL)
    {
        printf("\nUser File Error.\n");
        return;
    }


    while(fscanf(fp,
    "%d|%49[^|]|%32[^|]|%14[^|]|%19[^\n]",
    &u.id,
    u.username,
    u.password,
    u.role,
    u.phone)==5)
    {

        if(strcmp(u.username,currentUser)==0)
        {

            printf("\n=====================================\n");
            printf("             USER PROFILE\n");
            printf("=====================================\n");

            printf("User ID      : %d\n",u.id);
            printf("Username     : %s\n",u.username);
            printf("Role         : %s\n",u.role);
            printf("Phone        : %s\n",u.phone);

            printf("=====================================\n");


            fclose(fp);
            return;
        }
    }


    fclose(fp);

    printf("\nProfile Not Found.\n");
} 
/*====================================================
                CHANGE PASSWORD
=====================================================*/

void changePassword()
{
    if(strlen(currentUser)==0)
    {
        printf("\nPlease Login First.\n");
        return;
    }


    FILE *fp;
    FILE *temp;

    User u;

    char oldPass[50];
    char newPass[50];

    char oldHash[33];
    char newHash[33];

    int updated=0;


    printf("\n=====================================\n");
    printf("           CHANGE PASSWORD\n");
    printf("=====================================\n");


    printf("Old Password : ");
    scanf(" %[^\n]",oldPass);


    md5Hash(oldPass,oldHash);



    fp=fopen(USER_FILE,"r");

    temp=fopen("temp_user.txt","w");


    if(fp==NULL || temp==NULL)
    {
        printf("\nFile Error.\n");
        return;
    }



    while(fscanf(fp,
    "%d|%49[^|]|%32[^|]|%14[^|]|%19[^\n]",
    &u.id,
    u.username,
    u.password,
    u.role,
    u.phone)==5)
    {


        if(strcmp(u.username,currentUser)==0 &&
           strcmp(u.password,oldHash)==0)
        {

            printf("New Password : ");
            scanf(" %[^\n]",newPass);


            md5Hash(newPass,newHash);


            strcpy(u.password,newHash);


            updated=1;
        }



        fprintf(temp,
        "%d|%s|%s|%s|%s\n",
        u.id,
        u.username,
        u.password,
        u.role,
        u.phone);

    }


    fclose(fp);
    fclose(temp);



    remove(USER_FILE);
    rename("temp_user.txt",USER_FILE);



    if(updated)
    {
        printf("\nPassword Changed Successfully.\n");
    }
    else
    {
        printf("\nOld Password Incorrect.\n");
    }

}
/*====================================================
                FORGOT PASSWORD
=====================================================*/

void forgotPassword()
{

    FILE *fp;
    FILE *temp;


    User u;


    char username[50];
    char phone[20];

    char newPass[50];
    char hash[33];


    int found=0;



    printf("\n=====================================\n");
    printf("            FORGOT PASSWORD\n");
    printf("=====================================\n");


    printf("Username : ");
    scanf(" %[^\n]",username);


    printf("Phone Number : ");
    scanf(" %[^\n]",phone);



    fp=fopen(USER_FILE,"r");

    temp=fopen("temp_user.txt","w");



    if(fp==NULL || temp==NULL)
    {
        printf("\nFile Error.\n");
        return;
    }



    while(fscanf(fp,
    "%d|%49[^|]|%32[^|]|%14[^|]|%19[^\n]",
    &u.id,
    u.username,
    u.password,
    u.role,
    u.phone)==5)
    {


        if(strcmp(u.username,username)==0 &&
           strcmp(u.phone,phone)==0)
        {


            printf("New Password : ");
            scanf(" %[^\n]",newPass);



            md5Hash(newPass,hash);


            strcpy(u.password,hash);


            found=1;

        }



        fprintf(temp,
        "%d|%s|%s|%s|%s\n",
        u.id,
        u.username,
        u.password,
        u.role,
        u.phone);

    }



    fclose(fp);
    fclose(temp);



    remove(USER_FILE);
    rename("temp_user.txt",USER_FILE);



    if(found)
    {
        printf("\nPassword Reset Successful.\n");
    }
    else
    {
        printf("\nUsername or Phone Incorrect.\n");
    }

}
/*====================================================
                ADD VEHICLE
=====================================================*/
void addVehicle()
{
    if(strcmp(currentRole,"owner")!=0 &&
       strcmp(currentRole,"admin")!=0)
    {
        printf("\nOnly Owner or Admin can add vehicle.\n");
        return;
    }

    FILE *fp;
    Vehicle v, temp;

    int duplicate = 0;
    int choice;

    fp = fopen(VEHICLE_FILE, "a+");

    if(fp == NULL)
    {
        printf("\nVehicle File Error.\n");
        return;
    }

    printf("\n=====================================\n");
    printf("            ADD VEHICLE\n");
    printf("=====================================\n");

    printf("Vehicle ID : ");
    scanf("%d", &v.id);

    rewind(fp);

    while(fscanf(fp,
        "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%29[^|]|%d|%d|%f",
        &temp.id,
        &temp.ownerId,
        temp.type,
        temp.company,
        temp.from,
        temp.to,
        temp.departure,
        &temp.totalSeats,
        &temp.bookedSeats,
        &temp.fare) == 10)
    {
        if(temp.id == v.id)
        {
            duplicate = 1;
            break;
        }
    }

    if(duplicate)
    {
        printf("\nVehicle ID Already Exists.\n");
        fclose(fp);
        return;
    }

    v.ownerId = currentUserId;

    printf("\nSelect Vehicle Type\n");
    printf("1. Bus\n");
    printf("2. Train\n");
    printf("3. Ship\n");
    printf("4. Flight\n");

    printf("Choice : ");
    scanf("%d", &choice);

    switch(choice)
    {
        case 1:
            strcpy(v.type,"Bus");
            break;

        case 2:
            strcpy(v.type,"Train");
            break;

        case 3:
            strcpy(v.type,"Ship");
            break;

        case 4:
            strcpy(v.type,"Flight");
            break;

        default:
            printf("\nInvalid Choice.\n");
            fclose(fp);
            return;
    }

    getchar();

    printf("Company Name : ");
    fgets(v.company, sizeof(v.company), stdin);
    v.company[strcspn(v.company,"\n")] = '\0';

    printf("From : ");
    fgets(v.from, sizeof(v.from), stdin);
    v.from[strcspn(v.from,"\n")] = '\0';

    printf("To : ");
    fgets(v.to, sizeof(v.to), stdin);
    v.to[strcspn(v.to,"\n")] = '\0';

    printf("Departure Time : ");
    fgets(v.departure, sizeof(v.departure), stdin);
    v.departure[strcspn(v.departure,"\n")] = '\0'; 
        printf("\nEnter Total Seats : ");
    scanf("%d", &v.totalSeats);

    /*==============================
          SEAT VALIDATION
    ==============================*/

    if(strcmp(v.type,"Bus")==0)
    {
        if(v.totalSeats < 20 || v.totalSeats > 60)
        {
            printf("\nBus seat must be between 20 and 60.\n");
            fclose(fp);
            return;
        }
    }

    else if(strcmp(v.type,"Train")==0)
    {
        if(v.totalSeats < 40 || v.totalSeats > 120)
        {
            printf("\nTrain seat must be between 40 and 120.\n");
            fclose(fp);
            return;
        }
    }

    else if(strcmp(v.type,"Ship")==0)
    {
        if(v.totalSeats < 50 || v.totalSeats > 500)
        {
            printf("\nShip seat must be between 50 and 500.\n");
            fclose(fp);
            return;
        }
    }

    else if(strcmp(v.type,"Flight")==0)
    {
        if(v.totalSeats < 50 || v.totalSeats > 400)
        {
            printf("\nFlight seat must be between 50 and 400.\n");
            fclose(fp);
            return;
        }
    }

    v.bookedSeats = 0;

    /*==============================
          FARE INPUT
    ==============================*/

    printf("Fare : ");
    scanf("%f", &v.fare);

    if(v.fare <= 0)
    {
        printf("\nInvalid Fare.\n");
        fclose(fp);
        return;
    }

    /*==============================
         SAVE TO FILE
    ==============================*/

    fprintf(fp,
        "%d|%d|%s|%s|%s|%s|%s|%d|%d|%.2f\n",
        v.id,
        v.ownerId,
        v.type,
        v.company,
        v.from,
        v.to,
        v.departure,
        v.totalSeats,
        v.bookedSeats,
        v.fare); 
            /*==============================
            CLOSE FILE
    ==============================*/

    fclose(fp);

    /*==============================
          SUCCESS MESSAGE
    ==============================*/

    printf("\n=====================================\n");
    printf("     VEHICLE ADDED SUCCESSFULLY\n");
    printf("=====================================\n");

    printf("Vehicle ID      : %d\n", v.id);
    printf("Vehicle Type    : %s\n", v.type);
    printf("Company         : %s\n", v.company);
    printf("Route           : %s -> %s\n", v.from, v.to);
    printf("Departure Time  : %s\n", v.departure);
    printf("Total Seats     : %d\n", v.totalSeats);
    printf("Fare            : %.2f Tk\n", v.fare);

    printf("=====================================\n");
}
/*====================================================
                VIEW ALL VEHICLES
=====================================================*/

void viewVehicleByID()
{
    FILE *fp;
    Vehicle v;

    int id;
    int found=0;


    printf("\nEnter Vehicle ID : ");
    scanf("%d",&id);


    fp=fopen(VEHICLE_FILE,"r");


    if(fp==NULL)
    {
        printf("No Vehicle Found\n");
        return;
    }



    while(fscanf(fp,
    "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f",
    &v.id,
    &v.ownerId,
    v.type,
    v.company,
    v.from,
    v.to,
    v.departure,
    &v.totalSeats,
    &v.bookedSeats,
    &v.fare)==10)
    {


        if(v.id==id)
        {

            printf("\n==============================\n");
            printf("Vehicle ID : %d\n",v.id);
            printf("Type       : %s\n",v.type);
            printf("Company    : %s\n",v.company);
            printf("Route      : %s -> %s\n",v.from,v.to);
            printf("Departure  : %s\n",v.departure);
            printf("Seat       : %d/%d\n",
            v.bookedSeats,
            v.totalSeats);

            printf("Fare       : %.2f\n",v.fare);

            found=1;
            break;
        }

    }


    fclose(fp);


    if(!found)
    {
        printf("\nVehicle Not Found\n");
    }

}
/*====================================================
                UPDATE VEHICLE
=====================================================*/

void updateVehicle()
{
    if(strcmp(currentRole,"owner")!=0 &&
       strcmp(currentRole,"admin")!=0)
    {
        printf("\nOnly Owner or Admin can update vehicle.\n");
        return;
    }



    FILE *fp;
    FILE *temp;


    Vehicle v;


    int id;
    int found=0;



  //  viewVehicles();



    printf("\nEnter Vehicle ID to Update : ");
    scanf("%d",&id);



    fp=fopen(VEHICLE_FILE,"r");

    temp=fopen("temp_vehicle.txt","w");



    if(fp==NULL || temp==NULL)
    {
        printf("\nFile Error.\n");
        return;
    }



    while(fscanf(fp,
    "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f",
    &v.id,
    &v.ownerId,
    v.type,
    v.company,
    v.from,
    v.to,
    v.departure,
    &v.totalSeats,
    &v.bookedSeats,
    &v.fare)==10)
    {


        if(v.id==id)
        {

            if(strcmp(currentRole,"admin")==0 ||
               v.ownerId==currentUserId)
            {

                printf("\nNew Fare : ");
                scanf("%f",&v.fare);


                getchar();


                printf("New Departure Time : ");

                fgets(v.departure,
                      sizeof(v.departure),
                      stdin);


                v.departure[strcspn(v.departure,"\n")]=0;


                found=1;

            }

        }



        fprintf(temp,
        "%d|%d|%s|%s|%s|%s|%s|%d|%d|%.2f\n",
        v.id,
        v.ownerId,
        v.type,
        v.company,
        v.from,
        v.to,
        v.departure,
        v.totalSeats,
        v.bookedSeats,
        v.fare);

    }



    fclose(fp);

    fclose(temp);



    remove(VEHICLE_FILE);

    rename("temp_vehicle.txt",VEHICLE_FILE);



    if(found)
    {
        printf("\nVehicle Updated Successfully.\n");
    }
    else
    {
        printf("\nVehicle Not Found or Access Denied.\n");
    }

}
/*====================================================
                DELETE VEHICLE
=====================================================*/

void deleteVehicle()
{
    if(strcmp(currentRole,"owner")!=0 &&
       strcmp(currentRole,"admin")!=0)
    {
        printf("\nOnly Owner or Admin can delete vehicle.\n");
        return;
    }



    FILE *fp;
    FILE *temp;


    Vehicle v;


    int id;
    int found=0;



  //  viewVehicles();



    printf("\nEnter Vehicle ID to Delete : ");
    scanf("%d",&id);



    fp=fopen(VEHICLE_FILE,"r");

    temp=fopen("temp_vehicle.txt","w");



    if(fp==NULL || temp==NULL)
    {
        printf("\nFile Error.\n");
        return;
    }



    while(fscanf(fp,
    "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f",
    &v.id,
    &v.ownerId,
    v.type,
    v.company,
    v.from,
    v.to,
    v.departure,
    &v.totalSeats,
    &v.bookedSeats,
    &v.fare)==10)
    {


        if(v.id==id)
        {

            if(strcmp(currentRole,"admin")==0 ||
               v.ownerId==currentUserId)
            {

                found=1;

                continue;

            }

        }



        fprintf(temp,
        "%d|%d|%s|%s|%s|%s|%s|%d|%d|%.2f\n",
        v.id,
        v.ownerId,
        v.type,
        v.company,
        v.from,
        v.to,
        v.departure,
        v.totalSeats,
        v.bookedSeats,
        v.fare);

    }



    fclose(fp);

    fclose(temp);



    remove(VEHICLE_FILE);

    rename("temp_vehicle.txt",VEHICLE_FILE);



    if(found)
    {
        printf("\nVehicle Deleted Successfully.\n");
    }
    else
    {
        printf("\nVehicle Not Found or Access Denied.\n");
    }

}
/*====================================================
                SEARCH VEHICLE
=====================================================*/

void searchVehicle()
{
    FILE *fp;
    Vehicle v;

    int id;
    int found=0;


    printf("\nEnter Vehicle ID : ");
    scanf("%d",&id);


    fp=fopen(VEHICLE_FILE,"r");


    while(fscanf(fp,
    "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f",
    &v.id,
    &v.ownerId,
    v.type,
    v.company,
    v.from,
    v.to,
    v.departure,
    &v.totalSeats,
    &v.bookedSeats,
    &v.fare)==10)
    {


        if(v.id==id)
        {

            printf("\nVehicle Found\n");

            printf("Company : %s\n",v.company);
            printf("Route : %s -> %s\n",
            v.from,v.to);

            printf("Seat : %d/%d\n",
            v.bookedSeats,
            v.totalSeats);


            found=1;
            break;
        }

    }


    fclose(fp);


    if(!found)
        printf("Vehicle Not Found\n");

}
/*====================================================
                BOOK TICKET PART 5A-1
=====================================================*/
void bookTicket()
{
    if(strlen(currentUser)==0)
    {
        printf("\nPlease Login First.\n");
        return;
    }

    FILE *fp;
    Vehicle v;

    char from[50];
    char to[50];

    int vehicleId;
    int found = 0;

    printf("\n=====================================\n");
    printf("           BOOK TICKET\n");
    printf("=====================================\n");

    getchar();

    printf("From : ");
    fgets(from,sizeof(from),stdin);
    from[strcspn(from,"\n")] = 0;

    printf("To : ");
    fgets(to,sizeof(to),stdin);
    to[strcspn(to,"\n")] = 0;

    fp = fopen(VEHICLE_FILE,"r");

    if(fp==NULL)
    {
        printf("\nNo Vehicle Available.\n");
        return;
    }

    printf("\n====================================================================================\n");
    printf("%-5s %-10s %-20s %-15s %-10s %-10s\n",
           "ID","Type","Company","Departure","Seats","Fare");
    printf("====================================================================================\n");

    while(fscanf(fp,
    "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f",
    &v.id,
    &v.ownerId,
    v.type,
    v.company,
    v.from,
    v.to,
    v.departure,
    &v.totalSeats,
    &v.bookedSeats,
    &v.fare)==10)
    {
        if(strcasecmp(v.from,from)==0 &&
           strcasecmp(v.to,to)==0)
        {
            printf("%-5d %-10s %-20s %-15s %d/%d      %.2f\n",
                   v.id,
                   v.type,
                   v.company,
                   v.departure,
                   v.bookedSeats,
                   v.totalSeats,
                   v.fare);

            found = 1;
        }
    }

    fclose(fp);

    if(found==0)
    {
        printf("\nNo Vehicle Found.\n");
        return;
    }

    printf("\nEnter Vehicle ID : ");
    scanf("%d",&vehicleId);

    found = 0;

    fp = fopen(VEHICLE_FILE,"r");

    while(fscanf(fp,
    "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f",
    &v.id,
    &v.ownerId,
    v.type,
    v.company,
    v.from,
    v.to,
    v.departure,
    &v.totalSeats,
    &v.bookedSeats,
    &v.fare)==10)
    {
        if(v.id==vehicleId &&
           strcasecmp(v.from,from)==0 &&
           strcasecmp(v.to,to)==0)
        {
            found=1;
            break;
        }
    }

    fclose(fp);

    if(found==0)
    {
        printf("\nInvalid Vehicle ID.\n");
        return;
    }    int ticketCount;

    printf("\nHow Many Tickets : ");
    scanf("%d",&ticketCount);

    if(ticketCount < 1)
    {
        printf("\nInvalid Ticket Number.\n");
        return;
    }

    if(v.bookedSeats + ticketCount > v.totalSeats)
    {
        printf("\nOnly %d Seat Available.\n",
               v.totalSeats - v.bookedSeats);
        return;
    }

    Booking b,temp;

    getchar();

    strcpy(b.username,currentUser);

    printf("Passenger Name : ");
    fgets(b.passenger,sizeof(b.passenger),stdin);
    b.passenger[strcspn(b.passenger,"\n")] = 0;

    printf("Phone Number : ");
    fgets(b.phone,sizeof(b.phone),stdin);
    b.phone[strcspn(b.phone,"\n")] = 0;

    printf("Journey Date : ");
    fgets(b.journeyDate,sizeof(b.journeyDate),stdin);
    b.journeyDate[strcspn(b.journeyDate,"\n")] = 0;

    b.vehicleId = vehicleId;
    b.amount = v.fare;

    strcpy(b.status,"Confirmed");

    printf("\n========== PAYMENT ==========\n");
    printf("1. bKash\n");
    printf("2. Nagad\n");
    printf("3. Card\n");

    int pay;

    printf("Choice : ");
    scanf("%d",&pay);

    getchar();

    switch(pay)
    {
        case 1:
            strcpy(b.paymentMethod,"bKash");
            break;

        case 2:
            strcpy(b.paymentMethod,"Nagad");
            break;

        case 3:
            strcpy(b.paymentMethod,"Card");
            break;

        default:
            strcpy(b.paymentMethod,"Cash");
    }

    printf("Transaction ID : ");
    fgets(b.trxID,sizeof(b.trxID),stdin);
    b.trxID[strcspn(b.trxID,"\n")] = 0;

    FILE *bf;

    bf = fopen(BOOKING_FILE,"a+");

    if(bf==NULL)
    {
        printf("\nBooking File Error.\n");
        return;
    }

    int maxId = 1000;

    rewind(bf);

    while(fscanf(bf,
    "%d|%d|%49[^|]|%49[^|]|%19[^|]|%19[^|]|%d|%f|%19[^|]|%29[^|]|%19[^\n]",
    &temp.bookingId,
    &temp.vehicleId,
    temp.username,
    temp.passenger,
    temp.phone,
    temp.journeyDate,
    &temp.seatNo,
    &temp.amount,
    temp.paymentMethod,
    temp.trxID,
    temp.status)==11)
    {
        if(temp.bookingId > maxId)
        {
            maxId = temp.bookingId;
        }
    }    printf("\n========== SELECT SEATS ==========\n");

    int selectedSeats[MAX_SEATS];

    for(int i=0; i<ticketCount; i++)
    {
        int seat;
        int duplicate;
        int booked;

        while(1)
        {
            duplicate = 0;
            booked = 0;

            printf("Seat Number for Ticket %d : ",i+1);
            scanf("%d",&seat);

            if(seat<1 || seat>v.totalSeats)
            {
                printf("Invalid Seat Number.\n");
                continue;
            }

            /* Duplicate seat in same booking */
            for(int j=0; j<i; j++)
            {
                if(selectedSeats[j]==seat)
                {
                    duplicate=1;
                    break;
                }
            }

            if(duplicate)
            {
                printf("You already selected this seat.\n");
                continue;
            }

            /* Check already booked seat */
            rewind(bf);

            while(fscanf(bf,
            "%d|%d|%49[^|]|%49[^|]|%19[^|]|%19[^|]|%d|%f|%19[^|]|%29[^|]|%19[^\n]",
            &temp.bookingId,
            &temp.vehicleId,
            temp.username,
            temp.passenger,
            temp.phone,
            temp.journeyDate,
            &temp.seatNo,
            &temp.amount,
            temp.paymentMethod,
            temp.trxID,
            temp.status)==11)
            {
                if(temp.vehicleId==vehicleId &&
                   temp.seatNo==seat &&
                   strcmp(temp.status,"Confirmed")==0)
                {
                    booked=1;
                    break;
                }
            }

            if(booked)
            {
                printf("Seat Already Booked.\n");
                continue;
            }

            selectedSeats[i]=seat;

            b.bookingId=++maxId;
            b.seatNo=seat;

            fprintf(bf,
            "%d|%d|%s|%s|%s|%s|%d|%.2f|%s|%s|%s\n",
            b.bookingId,
            b.vehicleId,
            b.username,
            b.passenger,
            b.phone,
            b.journeyDate,
            b.seatNo,
            b.amount,
            b.paymentMethod,
            b.trxID,
            b.status);

            printf("Ticket %d Booked Successfully. Booking ID : %d\n",
                   i+1,
                   b.bookingId);

            break;
        }
    }

    fclose(bf);

    /* Update Vehicle Booked Seats */

    fp = fopen(VEHICLE_FILE,"r");
    FILE *tempVehicle = fopen("temp_vehicle.txt","w");

    while(fscanf(fp,
    "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f",
    &v.id,
    &v.ownerId,
    v.type,
    v.company,
    v.from,
    v.to,
    v.departure,
    &v.totalSeats,
    &v.bookedSeats,
    &v.fare)==10)
    {
        if(v.id==vehicleId)
        {
            v.bookedSeats += ticketCount;
        }

        fprintf(tempVehicle,
        "%d|%d|%s|%s|%s|%s|%s|%d|%d|%.2f\n",
        v.id,
        v.ownerId,
        v.type,
        v.company,
        v.from,
        v.to,
        v.departure,
        v.totalSeats,
        v.bookedSeats,
        v.fare);
    }

    fclose(fp);
    fclose(tempVehicle);

    remove(VEHICLE_FILE);
    rename("temp_vehicle.txt",VEHICLE_FILE);

    printf("\n========================================\n");
    printf("        BOOKING SUCCESSFUL\n");
    printf("========================================\n");

    printf("Passenger Name : %s\n",b.passenger);
    printf("Vehicle ID     : %d\n",vehicleId);
    printf("Total Tickets  : %d\n",ticketCount);

    printf("\nBooked Seats : ");

    for(int i=0;i<ticketCount;i++)
    {
        printf("%d ",selectedSeats[i]);
    }

    printf("\n");

    printf("Fare Per Ticket : %.2f Tk\n",v.fare);
    printf("Total Amount    : %.2f Tk\n",ticketCount*v.fare);
    printf("Payment Method  : %s\n",b.paymentMethod);

    printf("========================================\n");
}void displaySeatMap(int vehicleId, int totalSeats)
{
    FILE *fp;
    Booking b;

    int booked[MAX_SEATS + 1] = {0};

    fp = fopen(BOOKING_FILE,"r");

    if(fp == NULL)
    {
        printf("\nNo Booking Found.\n");
        return;
    }

    while(fscanf(fp,
    "%d|%d|%49[^|]|%49[^|]|%19[^|]|%19[^|]|%d|%f|%19[^|]|%29[^|]|%19[^\n]",
    &b.bookingId,
    &b.vehicleId,
    b.username,
    b.passenger,
    b.phone,
    b.journeyDate,
    &b.seatNo,
    &b.amount,
    b.paymentMethod,
    b.trxID,
    b.status)==11)
    {
        if(b.vehicleId == vehicleId &&
           strcmp(b.status,"Confirmed")==0)
        {
            booked[b.seatNo] = 1;
        }
    }

    fclose(fp);

    printf("\n============= SEAT MAP =============\n");

    for(int i=1;i<=totalSeats;i++)
    {
        if(booked[i])
            printf("[XX] ");
        else
            printf("[%02d] ",i);

        if(i%4==0)
            printf("\n");
    }

    printf("\n");
}void printTicket(Booking b)
{
    printf("\n");
    printf("=========================================\n");
    printf("          TRANSPORT TICKET\n");
    printf("=========================================\n");

    printf("Booking ID     : %d\n", b.bookingId);
    printf("Vehicle ID     : %d\n", b.vehicleId);
    printf("Username       : %s\n", b.username);
    printf("Passenger Name : %s\n", b.passenger);
    printf("Phone          : %s\n", b.phone);
    printf("Journey Date   : %s\n", b.journeyDate);
    printf("Seat Number    : %d\n", b.seatNo);
    printf("Fare           : %.2f Tk\n", b.amount);
    printf("Payment Method : %s\n", b.paymentMethod);
    printf("Transaction ID : %s\n", b.trxID);
    printf("Status         : %s\n", b.status);

    printf("=========================================\n");
}
/*====================================================
             VIEW MY BOOKINGS
=====================================================*/

void viewBookings()
{
    if(strlen(currentUser)==0)
    {
        printf("\nPlease Login First.\n");
        return;
    }



    FILE *fp;

    Booking b;


    int count=0;



    fp=fopen(BOOKING_FILE,"r");



    if(fp==NULL)
    {
        printf("\nNo Booking Found.\n");
        return;
    }



    printf("\n============================================\n");
    printf("              MY BOOKING LIST                \n");
    printf("============================================\n");



    while(fscanf(fp,
    "%d|%d|%49[^|]|%49[^|]|%19[^|]|%14[^|]|%d|%f|%19[^|]|%19[^|]|%19[^\n]",
    &b.bookingId,
    &b.vehicleId,
    b.username,
    b.passenger,
    b.phone,
    b.journeyDate,
    &b.seatNo,
    &b.amount,
    b.paymentMethod,
    b.trxID,
    b.status)==11)
    {


        if(strcmp(currentRole,"admin")==0 ||
           strcmp(b.username,currentUser)==0)
        {


            printTicket(b);

            count++;

        }


    }



    fclose(fp);



    if(count==0)
    {
        printf("\nNo Booking Available.\n");
    }

}
/*====================================================
                CANCEL TICKET
=====================================================*/

void cancelTicket()
{
    if(strlen(currentUser)==0)
    {
        printf("\nPlease Login First.\n");
        return;
    }



    FILE *fp;
    FILE *temp;


    Booking b;


    int bookingId;
    int found=0;

    int vehicleId=0;



    printf("\n=====================================\n");
    printf("            CANCEL TICKET\n");
    printf("=====================================\n");



    printf("Enter Booking ID : ");
    scanf("%d",&bookingId);



    fp=fopen(BOOKING_FILE,"r");

    temp=fopen("temp_booking.txt","w");



    if(fp==NULL || temp==NULL)
    {
        printf("\nFile Error.\n");
        return;
    }



    while(fscanf(fp,
    "%d|%d|%49[^|]|%49[^|]|%19[^|]|%14[^|]|%d|%f|%19[^|]|%19[^|]|%19[^\n]",
    &b.bookingId,
    &b.vehicleId,
    b.username,
    b.passenger,
    b.phone,
    b.journeyDate,
    &b.seatNo,
    &b.amount,
    b.paymentMethod,
    b.trxID,
    b.status)==11)
    {


        if(b.bookingId==bookingId)
        {

            if(strcmp(currentRole,"admin")==0 ||
               strcmp(b.username,currentUser)==0)
            {

                strcpy(b.status,"Cancelled");

                vehicleId=b.vehicleId;

                found=1;

            }

        }



        fprintf(temp,
        "%d|%d|%s|%s|%s|%s|%d|%.2f|%s|%s|%s\n",
        b.bookingId,
        b.vehicleId,
        b.username,
        b.passenger,
        b.phone,
        b.journeyDate,
        b.seatNo,
        b.amount,
        b.paymentMethod,
        b.trxID,
        b.status);

    }



    fclose(fp);

    fclose(temp);



    remove(BOOKING_FILE);

    rename("temp_booking.txt",BOOKING_FILE);



    if(found)
    {

        /* UPDATE VEHICLE SEAT */

        fp=fopen(VEHICLE_FILE,"r");

        temp=fopen("temp_vehicle.txt","w");


        Vehicle v;


        while(fscanf(fp,
        "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f",
        &v.id,
        &v.ownerId,
        v.type,
        v.company,
        v.from,
        v.to,
        v.departure,
        &v.totalSeats,
        &v.bookedSeats,
        &v.fare)==10)
        {


            if(v.id==vehicleId &&
               v.bookedSeats>0)
            {

                v.bookedSeats--;

            }



            fprintf(temp,
            "%d|%d|%s|%s|%s|%s|%s|%d|%d|%.2f\n",
            v.id,
            v.ownerId,
            v.type,
            v.company,
            v.from,
            v.to,
            v.departure,
            v.totalSeats,
            v.bookedSeats,
            v.fare);

        }



        fclose(fp);

        fclose(temp);



        remove(VEHICLE_FILE);

        rename("temp_vehicle.txt",VEHICLE_FILE);



        printf("\nTicket Cancelled Successfully.\n");

    }

    else
    {
        printf("\nBooking Not Found or Access Denied.\n");
    }


}




/*====================================================
                OWNER DASHBOARD
=====================================================*/

void ownerDashboard()
{
    if(strcmp(currentRole,"owner")!=0 &&
       strcmp(currentRole,"admin")!=0)
    {
        printf("\nOnly Owner or Admin Access Allowed.\n");
        return;
    }



    FILE *fp;

    Vehicle v;


    int totalVehicle=0;
    int totalSeats=0;
    int totalBooked=0;


    float totalIncome=0;



    fp=fopen(VEHICLE_FILE,"r");



    if(fp==NULL)
    {
        printf("\nNo Vehicle Found.\n");
        return;
    }




    while(fscanf(fp,
    "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f",
    &v.id,
    &v.ownerId,
    v.type,
    v.company,
    v.from,
    v.to,
    v.departure,
    &v.totalSeats,
    &v.bookedSeats,
    &v.fare)==10)
    {


        if(strcmp(currentRole,"admin")==0 ||
           v.ownerId==currentUserId)
        {


            totalVehicle++;


            totalSeats += v.totalSeats;


            totalBooked += v.bookedSeats;


            totalIncome += (v.bookedSeats * v.fare);


        }


    }



    fclose(fp);



    printf("\n=========================================\n");
    printf("             OWNER DASHBOARD\n");
    printf("=========================================\n");


    printf("Total Vehicle       : %d\n",totalVehicle);

    printf("Total Seat Capacity : %d\n",totalSeats);

    printf("Total Booked Seat   : %d\n",totalBooked);

    printf("Total Income        : %.2f Tk\n",totalIncome);


    printf("=========================================\n");

}
/*====================================================
                ADMIN REPORT SYSTEM
=====================================================*/

void adminReport()
{
    if(strcmp(currentRole,"admin")!=0)
    {
        printf("\nOnly Admin Can Access This Report.\n");
        return;
    }



    FILE *fp;


    User u;

    Vehicle v;

    Booking b;



    int totalUser=0;
    int totalCustomer=0;
    int totalOwner=0;


    int totalVehicle=0;


    int totalBooking=0;
    int cancelled=0;


    float revenue=0;




    /* USER REPORT */

    fp=fopen(USER_FILE,"r");


    if(fp!=NULL)
    {

        while(fscanf(fp,
        "%d|%49[^|]|%32[^|]|%14[^|]|%19[^\n]",
        &u.id,
        u.username,
        u.password,
        u.role,
        u.phone)==5)
        {


            totalUser++;


            if(strcmp(u.role,"customer")==0)
                totalCustomer++;


            else if(strcmp(u.role,"owner")==0)
                totalOwner++;


        }


        fclose(fp);

    }




    /* VEHICLE REPORT */

    fp=fopen(VEHICLE_FILE,"r");


    if(fp!=NULL)
    {

        while(fscanf(fp,
        "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f",
        &v.id,
        &v.ownerId,
        v.type,
        v.company,
        v.from,
        v.to,
        v.departure,
        &v.totalSeats,
        &v.bookedSeats,
        &v.fare)==10)
        {

            totalVehicle++;

        }


        fclose(fp);

    }





    /* BOOKING REPORT */

    fp=fopen(BOOKING_FILE,"r");


    if(fp!=NULL)
    {

        while(fscanf(fp,
        "%d|%d|%49[^|]|%49[^|]|%19[^|]|%14[^|]|%d|%f|%19[^|]|%19[^|]|%19[^\n]",
        &b.bookingId,
        &b.vehicleId,
        b.username,
        b.passenger,
        b.phone,
        b.journeyDate,
        &b.seatNo,
        &b.amount,
        b.paymentMethod,
        b.trxID,
        b.status)==11)
        {


            totalBooking++;


            if(strcmp(b.status,"Confirmed")==0)
            {

                revenue += b.amount;

            }


            else if(strcmp(b.status,"Cancelled")==0)
            {

                cancelled++;

            }


        }


        fclose(fp);

    }




    printf("\n============================================\n");
    printf("              ADMIN SYSTEM REPORT\n");
    printf("============================================\n");


    printf("Total Users          : %d\n",totalUser);

    printf("Total Customers      : %d\n",totalCustomer);

    printf("Total Owners         : %d\n",totalOwner);

    printf("--------------------------------------------\n");

    printf("Total Vehicles       : %d\n",totalVehicle);

    printf("Total Bookings       : %d\n",totalBooking);

    printf("Cancelled Tickets    : %d\n",cancelled);

    printf("Total Revenue        : %.2f Tk\n",revenue);


    printf("============================================\n");

}
/*====================================================
                SYSTEM REFRESH
=====================================================*/

  void systemRefresh()
{

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif


    printf("\n=====================================\n");
    printf("          SYSTEM REFRESH\n");
    printf("=====================================\n");


    initAdmin();


    printf("\nSystem Refreshed Successfully.\n");


    printf("All modules are ready.\n");


    printf("=====================================\n");

}
/*====================================================
                 MAIN MENU SYSTEM
=====================================================*/

int main()
{

    int choice;


    initAdmin();



    while(1)
    {

        printf("\n\n");
        printf("=================================================\n");
        printf("      SMART TRANSPORT TICKET BOOKING SYSTEM      \n");
        printf("=================================================\n");


        if(strlen(currentUser)>0)
        {
            printf("Logged In User : %s\n",currentUser);
            printf("Role           : %s\n",currentRole);
        }
        else
        {
            printf("Status : Not Logged In\n");
        }


        printf("-------------------------------------------------\n");

        printf("1.  Customer Register\n");
        printf("2.  Customer Login\n");

        printf("3.  Owner Register\n");
        printf("4.  Owner Login\n");

        //printf("5.  Admin Register\n");
        printf("5.  Admin Login\n");

        printf("-------------------------------------------------\n");

        printf("6.  View Profile\n");

        printf("7.  Add Vehicle\n");
        printf("8.  View All Vehicles\n");
        printf("9. Update Vehicle\n");
        printf("10. Delete Vehicle\n");
        printf("11. Search Vehicle\n");

        printf("-------------------------------------------------\n");

        printf("12. Book Ticket\n");
        printf("13. View Booking\n");
        printf("14. Cancel Ticket\n");

        printf("-------------------------------------------------\n");

        printf("15. Owner Dashboard\n");
        printf("16. Admin Report\n");

        printf("17. Change Password\n");
        printf("18. System Refresh\n");

        printf("19. Logout\n");

        printf("20. Exit\n");


        printf("-------------------------------------------------\n");

        printf("Enter Choice : ");
        scanf("%d",&choice);



        switch(choice)
        {


            case 1:
                registerUser("customer");
                break;


            case 2:
                login("customer");
                break;



            case 3:
                registerUser("owner");
                break;



            case 4:
                login("owner");
                break;


/*
            case 5:
                registerUser("admin");
                break;*/



            case 5:
                login("admin");
                break;



            case 6:
                viewProfile();
                break;



            case 7:
                addVehicle();
                break;



            case 8:
                viewVehicleByID();
                 break;



            case 9:
                updateVehicle();
                break;



            case 10:
                deleteVehicle();
                break;



            case 11:
                searchVehicle();
                break;



            case 12:
                bookTicket();
                break;



            case 13:
                viewBookings();
                break;



            case 14:
                cancelTicket();
                break;



            case 15:
                ownerDashboard();
                break;



            case 16:
                adminReport();
                break;



            case 17:
                changePassword();
                break;



            case 18:
                systemRefresh();
                break;



            case 19:
                logout();
                break;



            case 20:

                printf("\nThank You For Using System.\n");

                exit(0);



            default:

                printf("\nInvalid Choice.\n");

        }


    }



    return 0;

}