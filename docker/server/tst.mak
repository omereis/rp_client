OS = 'LINUX'
myOS = 'docker'

ifeq ($(OS),$(myOS))
	message = 'Linux Operating System'
else
	message = 'Docker'
endif

install:
	echo $(message)
	#echo $(OS)

