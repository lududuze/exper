TOPDIR = /home/lyd/web/ws/07_httpd/
include $(TOPDIR)Rules.mak 
EXTRA_LIBS += -lpthread

EXEC = $(INSTALL_DIR)  ./httpd
OBJS = httpd.o copy.o

HTTPD_DOCUMENT_ROOT = /home/lyd/web/ws/07_httpd/
CFLAGS += -DHTTPD_DOCUMENT_ROOT=\"$(HTTPD_DOCUMENT_ROOT)\"

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(EXTRA_LIBS)


install:
	$(EXP_INSTALL) $(EXEC) $(INSTALL_DIR)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
