Simple File System

Developed by Miguel Sanseverino

Instructions:

- Start server with port
    ./server <port>

- Start client with address and port
    ./client <addres> <port>

-Request template

read:       RD-REQ*<path>*<nrbytes>*<offset>*<client_id>
write:      WR-REQ*<path>*<nrbytes>*<offset>*<client_id>
file:       FI-REQ*<path>
directory:  D<Option>-REQ*<path>*<dirname>*<client_id>