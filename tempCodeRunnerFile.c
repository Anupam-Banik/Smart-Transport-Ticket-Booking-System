#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define VEHICLE_FILE "vehicles.txt"
#define BOOKING_FILE "bookings.txt"
#define MAX_SEATS    100

typedef struct {
    int id;
    int ownerId;
    char type[20];
    char company[50];
    char from[50];
    char to[50];
    char departure[20];
    int totalSeats;
    int bookedSeats;
    float fare;
} Vehicle;

typedef struct {
    int bookingId;
    int vehicleId;
    char username[50];
    char passenger[50];
    char phone[20];
    char journeyDate[20];
    int seatNo;
    float amount;
    char paymentMethod[20];
    char trxID[30];
    char status[20];
} Booking;

char currentUser[50] = "user1";
char currentRole[20] = "user";

void searchVehicle();
void bookTicket();
void displaySeatMap(int vehicleId, int totalSeats);
void printTicket(Booking b);
void viewBookings();
void cancelTicket();

int main()
{
    int choice;

    while(1)
    {
        printf("\n=====================================\n");
        printf("   TRANSPORT TICKET BOOKING SYSTEM   \n");
        printf("=====================================\n");
        printf("1. Search Vehicle\n");
        printf("2. Book Ticket\n");
        printf("3. View My Bookings\n");
        printf("4. Cancel Ticket\n");
        printf("-------------------------------------\n");
        printf("Enter Choice: ");
        if(scanf("%d", &choice) != 1)
        {
            while(getchar() != '\n');
            continue;
        }

        switch(choice)
        {
            case 1:
                searchVehicle();
                break;
            case 2:
                bookTicket();
                break;
            case 3:
                viewBookings();
                break;
            case 4:
                cancelTicket();
                break;
            default:
                printf("\nInvalid choice.\n");
        }
    }
    return 0;
}

void searchVehicle()
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
        printf("Vehicle File Not Found\n");
        return;
    }


    while(fscanf(fp,
    "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f\n",
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
    "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f\n",
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
    "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f\n",
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
    }

    int ticketCount;

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
    "%d|%d|%49[^|]|%49[^|]|%19[^|]|%19[^|]|%d|%f|%19[^|]|%29[^|]|%19[^\n]\n",
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
    }

    printf("\n========== SELECT SEATS ==========\n");

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
            "%d|%d|%49[^|]|%49[^|]|%19[^|]|%19[^|]|%d|%f|%19[^|]|%29[^|]|%19[^\n]\n",
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
    "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f\n",
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
}

void displaySeatMap(int vehicleId, int totalSeats)
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
    "%d|%d|%49[^|]|%49[^|]|%19[^|]|%19[^|]|%d|%f|%19[^|]|%29[^|]|%19[^\n]\n",
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
            if(b.seatNo <= MAX_SEATS)
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
}

void printTicket(Booking b)
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
    "%d|%d|%49[^|]|%49[^|]|%19[^|]|%19[^|]|%d|%f|%19[^|]|%29[^|]|%19[^\n]\n",
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
        if(fp) fclose(fp);
        if(temp) fclose(temp);
        return;
    }



    while(fscanf(fp,
    "%d|%d|%49[^|]|%49[^|]|%19[^|]|%19[^|]|%d|%f|%19[^|]|%29[^|]|%19[^\n]\n",
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
        "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%d|%d|%f\n",
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