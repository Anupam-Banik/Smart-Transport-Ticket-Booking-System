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

}
Booking;
void viewProfile()
{
    if(strlen(currentUser)==0) //এখানে strlen() দিয়ে currentUser-এর length check করা হচ্ছে।
    {
        printf("\nPlease Login First.\n");
        return;
    }


    FILE *fp; //file pointer turi kora hoise ja file sathe kaj kore 

    User u; /*এখানে User structure-এর একটি variable u তৈরি হয়েছে।
                এটার মধ্যে temporarily একজন user-এর information রাখা হবে।*/

    fp=fopen(USER_FILE,"r");


    if(fp==NULL)
    {
        printf("\nUser File Error.\n");// file open na hile value null
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
    //Variable declaration

    FILE *fp;
    Vehicle v, temp;

    int duplicate = 0;//মানে এখনো duplicate পাওয়া যায়নি।
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

    rewind(fp);//rewind(fp) moves the file pointer back to the beginning of the file.

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
            //Duplicate vehicle ID prevent করার জন্য আগে existing IDs check করা হয়েছে।
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
//getchar() is used here to consume the leftover newline character from the input buffer before using fgets(
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


    Vehicle v;//File থেকে একটা vehicle-এর data v-এর মধ্যে রাখবে।


    int id;
    int found=0;



  //  viewVehicles();



    printf("\nEnter Vehicle ID to Update : ");
    scanf("%d",&id);



    fp=fopen(VEHICLE_FILE,"r");//vehicles.txt read mode-এ open হচ্ছে।

    temp=fopen("temp_vehicle.txt","w");//এখানে নতুন temporary file তৈরি হচ্ছে।



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

    rename("temp_vehicle.txt",VEHICLE_FILE);//updated file-টাই এখন original file হয়ে গেল।



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