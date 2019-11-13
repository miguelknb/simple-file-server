Simple File System

Developed by Miguel Sanseverino

Instructions:

- Start server with port
    ./server <port>

- Start client with address and port
    ./client <addres> <port>

-Request template
    RD-REQ*<path>*<nrbytes>*<offset>*<client_id>
    WR-REQ*<path>*<nrbytes>*<offset>*<client_id>
    FI-REQ*<path>
    D<Option>-REQ*<path>*<dirname>*<client_id>