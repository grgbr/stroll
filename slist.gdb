define _stroll_out
output $arg0
printf "\n"
end

define stroll-print
printf "*"
output $arg0
printf " = "
_stroll_out *$arg0
end

define stroll-offsetof
if $argc == 3
	set $_off = (unsigned long)(&(($arg0 $arg1 *)0)->$arg2)
else
	set $_off = (unsigned long)(&(($arg0 *)0)->$arg1)
end
_stroll_out $_off
end

document stroll-offsetof
Display offset of FIELD field within TYPE type.
Usage: stroll-offsetof TYPE FIELD
end

define stroll-entry
if $argc == 4
	set $_off = (unsigned long)(&(($arg0 $arg1 *)0)->$arg2)
	set $_addr = ($arg0 $arg1 *)((const char *)($arg3) - $_off)
else
	set $_off = (unsigned long)(&(($arg0 *)0)->$arg1)
	set $_addr = ($arg0 *)((const char *)($arg2) - $_off)
end
stroll-print $_addr
end

document stroll-entry
Display entry of type TYPE containing FIELD field pointed to by SYM.
Usage: stroll-entry TYPE FIELD SYM
end

define stroll-slist-next
stroll-print $arg0->next
end

document stroll-slist-next
Display node following stroll_slist_node NODE within a stroll_slist singly
linked list.
Usage: stroll-slist-next NODE
end

define stroll-slist-head
stroll-print &$arg0->head
end

document stroll-slist-head
Display head node of singly linked list stroll_slist LIST.
Usage: stroll-slist-head SLIST
end

define stroll-slist-tail
stroll-print $arg0->tail
end

document stroll-slist-tail
Display tail node of singly linked list stroll_slist LIST.
Usage: stroll-slist-tail SLIST
end

define stroll-slist-nodes
set $_node = $arg0->head.next
set $_idx = 0
printf "     <head> "
stroll-print &$arg0->head
while $_node != NULL
	printf "%10u: ", $_idx
	stroll-print $_node
	set $_node = $_node->next
	set $_idx = $_idx + 1
end
printf "     <tail> "
stroll-print $arg0->tail
end

document stroll-slist-nodes
Display all nodes of singly linked list stroll_slist LIST.
Usage: stroll-slist-nodes SLIST
end

define stroll-slist-entries
if $argc == 4
	set $_lst = $arg3
else
	set $_lst = $arg2
end
set $_node = $_lst->head.next
set $_idx = 0
printf "     <head> "
stroll-print &$_lst->head
while $_node != NULL
	printf "%10u: ", $_idx
	if $argc == 4
		stroll-entry $arg0 $arg1 $arg2 $_node
	else
		stroll-entry $arg0 $arg1 $_node
	end

	set $_node = $_node->next
	set $_idx = $_idx + 1
end
printf "     <tail> "
stroll-print $_lst->tail
end

document stroll-slist-entries
Display all entries of type TYPE containing FIELD field linked into
the stroll_slist list SLIST.
Usage: stroll-slist-entries TYPE FIELD SLIST
end

define stroll-slist-bubble
stroll-slist-entries struct strollut_slist_node super list
if sort != NULL
	printf "sort: "
	stroll-entry struct strollut_slist_node super sort
else
	printf "sort: NULL\n"
end
if swap != NULL
	printf "swap: "
	stroll-entry struct strollut_slist_node super swap
else
	printf "swap: NULL\n"
end
if prev != NULL
	printf "prev: "
	stroll-entry struct strollut_slist_node super prev
else
	printf "prev: NULL\n"
end
if curr != NULL
	printf "curr: "
	stroll-entry struct strollut_slist_node super curr
else
	printf "curr: NULL\n"
end
if next != NULL
	printf "next: "
	stroll-entry struct strollut_slist_node super next
else
	printf "next: NULL\n"
end
end

define stroll-slist-bubble-next
next
stroll-slist-bubble
end

define stroll-slist-bubble-cont
continue
stroll-slist-bubble
end
