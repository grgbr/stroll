################################################################################
# SPDX-License-Identifier: GPL-3.0-only
#
# This file is part of Stroll.
# Copyright (C) 2017-2024 Grégor Boirie <gregor.boirie@free.fr>
#
# Stroll library GDB helper macro definitions.
################################################################################

################################################################################
# Common utilities
################################################################################

define-prefix stroll

document stroll
Stroll library macros.
Type `help stroll' for more informations.
end


define _stroll_out
output $arg0
printf "\n"
end

define stroll print
printf "*"
output $arg0
printf " = "
_stroll_out *$arg0
end

document stroll print
Display address and content of SYM pointer.
Usage: stroll print SYM
end

define stroll offsetof
if $argc == 3
	set $_off = (unsigned long)(&(($arg0 $arg1 *)0)->$arg2)
else
	set $_off = (unsigned long)(&(($arg0 *)0)->$arg1)
end
_stroll_out $_off
end

document stroll offsetof
Display offset of FIELD field within TYPE type.
Usage: stroll offsetof TYPE FIELD
end

define stroll entry
if $argc == 4
	set $_off = (unsigned long)(&(($arg0 $arg1 *)0)->$arg2)
	set $_addr = ($arg0 $arg1 *)((const char *)($arg3) - $_off)
else
	set $_off = (unsigned long)(&(($arg0 *)0)->$arg1)
	set $_addr = ($arg0 *)((const char *)($arg2) - $_off)
end
stroll print $_addr
end

document stroll entry
Display entry of type TYPE containing FIELD field pointed to by SYM.
Usage: stroll entry TYPE FIELD SYM
end

################################################################################
# stroll_slist singly linked list macros.
################################################################################

define-prefix stroll slist

document stroll slist
Stroll library stroll_slist singly linked list related macros.
Type `help stroll slist' for more informations.
end

define stroll slist next
stroll print $arg0->next
end

document stroll slist next
Display node following stroll_slist_node NODE pointer within a stroll_slist \
singly linked list.
Usage: stroll slist next NODE
end

define stroll slist head
stroll print &$arg0->head
end

document stroll slist head
Display head node of stroll_slist singly linked list LIST pointer.
Usage: stroll slist head SLIST
end

define stroll slist tail
stroll print $arg0->tail
end

document stroll slist tail
Display tail node of stroll_slist singly linked list LIST pointer.
Usage: stroll slist tail SLIST
end

define stroll slist nodes
set $_node = $arg0->head.next
set $_idx = 0
printf "     <head> "
stroll print &$arg0->head
while $_node
	printf "%10u: ", $_idx
	stroll print $_node
	set $_node = $_node->next
	set $_idx = $_idx + 1
end
printf "     <tail> "
stroll print $arg0->tail
end

document stroll slist nodes
Display all nodes of stroll_slist singly linked list LIST pointer.
Usage: stroll slist nodes SLIST
end

define stroll slist entries
if $argc == 4
	set $_lst = $arg3
else
	set $_lst = $arg2
end
set $_node = $_lst->head.next
set $_idx = 0
printf "     <head> "
stroll print &$_lst->head
while $_node
	printf "%10u: ", $_idx
	if $argc == 4
		stroll entry $arg0 $arg1 $arg2 $_node
	else
		stroll entry $arg0 $arg1 $_node
	end

	set $_node = $_node->next
	set $_idx = $_idx + 1
end
printf "     <tail> "
stroll print $_lst->tail
end

document stroll slist entries
Display all entries of type TYPE containing FIELD field linked into \
the stroll_slist singly linked list SLIST pointer.
Usage: stroll slist entries TYPE FIELD SLIST
end

################################################################################
# stroll_dlist doubly linked list macros.
################################################################################

define-prefix stroll dlist

document stroll dlist
Stroll library stroll_dlist_node doubly linked list related macros.
Type `help stroll dlist' for more informations.
end

define stroll dlist next
stroll print $arg0->next
end

document stroll dlist next
Display node following stroll_dlist_node NODE pointer within a \
stroll_dlist_node doubly linked list.
Usage: stroll dlist next NODE
end

define stroll dlist prev
stroll print $arg0->prev
end

document stroll dlist prev
Display node preceeding stroll_dlist_node NODE pointer within a \
stroll_dlist_node doubly linked list.
Usage: stroll dlist prev NODE
end

define stroll dlist nodes
set $_node = $arg0->next
set $_idx = 0
printf "     <head> "
stroll print $arg0
while $_node != $arg0
	printf "%10u: ", $_idx
	stroll print $_node
	set $_node = $_node->next
	set $_idx = $_idx + 1
end
end

document stroll dlist nodes
Display all nodes of stroll_dlist_node doubly linked list which head is given \
as pointer argument HEAD.
Usage: stroll dlist nodes HEAD
end

define stroll dlist entries
if $argc == 4
	set $_head = $arg3
else
	set $_head = $arg2
end
set $_node = $_head->next
set $_idx = 0
printf "     <head> "
stroll print $_head
while $_node != $_head
	printf "%10u: ", $_idx
	if $argc == 4
		stroll entry $arg0 $arg1 $arg2 $_node
	else
		stroll entry $arg0 $arg1 $_node
	end

	set $_node = $_node->next
	set $_idx = $_idx + 1
end
end

document stroll dlist entries
Display all entries of type TYPE containing FIELD field linked into \
the stroll_dlist_node doubly linked list which head is given as pointer \
argument HEAD.
Usage: stroll dlist entries TYPE FIELD HEAD
end

################################################################################
# Default GDB settings.
################################################################################

set history save on
set history size 1024
set history remove-duplicates 16

set print symbol-filename on
set print array on

# ex: filetype=gdb
