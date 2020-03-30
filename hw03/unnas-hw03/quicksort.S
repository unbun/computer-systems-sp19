.global main
.data
bad_input: .string "bad input"
int_fmt: .string "%ld"
int_fmt_nl: .string "%ld\n"
int_fmt_s: .string "%ld "
null_str:		.string ""

response_int_fmt: .string "entered: %ld\n"

// hello 
.text
main:
	push %r13
	push %r12
	enter $0, $0

	// long nn;        // nn --> %r13 
	// read_int(&nn);  

	mov %r13, %rdi
	call read_int
	mov (%r13), %r13


	// calculate byte size of xs
    mov $8, %r10
    imul %r10

	// calculate byte size of xs
    mov $8, %r10
    imul %r10

	// xs is the address of the array  // xs ---> %r12
	// long* xs = malloc(nn * sizeof(long)); //
    sub %rax, %rsp
    mov %rsp, %r12

    // Force stack to be aligned by maybe
    // allocating another 8 bytes of space.
    mov $15, %r10
    not %r10
    and %r10, %rsp

	// for (long ii = 0; ii < nn; ++ii) { // ii--> %rcx
	//     read_int(&(xs[ii]));
	// }
	mov $0, %rcx

main_read_loop_cond:
	cmp %r13, %rcx
	jge main_read_loop_done

	push %rcx
	push %rcx
	lea (%r12, %rcx, 8), %rdi
	call read_int
	pop %rcx
	pop %rcx

	inc %rcx
	jmp main_read_loop_cond

main_read_loop_done:

	// quicksort(xs, 0, nn);
	mov %r12, %rdi
	mov $0, %rsi
	mov %r13, %rdx
	call quicksort


	// printf("%ld\n", nn);
	mov %r13, %rsi
	mov $int_fmt_nl, %rdi
	call printf

    mov $0, %rax


    // for (long ii = 0; ii < nn; ++ii) {
	//     printf("%ld ", xs[ii]);
	// }
print_loop_cond:
	cmp %r13, %rcx
	jge print_loop_done

	push %rcx
	push %rcx

	lea (%r12, %rcx, 8), %rsi
	mov $int_fmt_s, %rdi
	call printf

	pop %rcx
	pop %rcx

	inc %rcx
	jmp print_loop_cond

print_loop_done:
	
	// printf("\n");
	mov $null_str, %rdi
	call puts

	 

	// free(xs);
  	leave
	pop %r12
	pop %r13
	ret

read_int:
	enter $0, $0
    // if (scanf("%ld", ptr) != 1) { //  ptr --> %rdi
    //     puts("bad input");
    //     exit(1);
    // }

	lea (%rdi), %rsi
	mov $int_fmt, %rdi
	mov $0, %al
	call scanf
	mov (%rdi), %rdi

	cmp $1, %rax
	je read_int_done

	mov $bad_input, %rdi
	call puts

	mov $1, %rdi
	call exit
	
read_int_done:
	leave
	ret

quicksort:
	push %r15
	push %r14
	push %r13
	push %r12
	enter $0, $0

	mov %rdi, %r14 // xs
	mov %rsi, %r13 // lo
	mov %rdx, %r12 // hi

	// xs[ii] --> mov (%r14, %rii, 8), %rxx
	
	// if (hi - lo < 1) {
	//   return;
	// }
	mov %r12, %r10
	sub %r13, %r10
	cmp $1, %r10
	jl quicksort_done

	// long pp = partition(xs, lo, hi); // pp --> %r15
	lea (%r14), %rdi
	mov %r13, %rsi
	mov %r12, %rdx
	call partition;
	mov %rax, %r15

	mov %r14, %rdi
	mov %r12, %rsi
	mov %r15, %rdx
	call quicksort;

	inc %r15
	mov %r14, %rdi
	mov %r15, %rsi
	mov %r12, %rdx
	call quicksort;

quicksort_done:
	leave
	pop %r12
	pop %r13
	pop %r14
	pop %r15
	ret

partition:
	push %r15
	push %r14
	push %r13
	push %r12
	enter $0, $0

	mov %rdi, %r15 // xs --> %r15
	mov %rdx, %r14 // (hi - 1) --> %r14
	mov %rsi, %r12 // lo, ii --> %r12
	mov %rsi, %rcx // lo, jj --> %r11

	// %rdi --> long *xs
	// %rsi --> long lo
	// %rdx --> long hi

	// xs[ii] --> mov (%rdi, %rii, 8), %rxx

	// pivot = xs[hi - 1];  // pivot --> %r13
	dec %r14
	lea (%rdi, %rdx, 8), %r13

	// for (; jj < (hi - 1); ++jj) {
	//     if (xs[jj] < pivot) {
	//         swap(xs, ii, jj);
	//         ++ii;
	//     }
	// }

// for (; jj < (hi - 1); ++jj) {
part_loop_cond:
	cmp %rcx, %r14
	jg part_loop_done

	push %rcx
	push %rcx

	// if (xs[jj] < pivot)
	mov (%r15, %rcx, 8), %r10
	cmp %r10, %r13
	jg part_loop_next

	// swap(xs, ii, jj);
	mov %r15, %rdi
	mov %r12, %rsi
	mov %rcx, %rdx
	call swap

	// 	++ii;
	inc %r12

part_loop_next:
	pop %rcx
	pop %rcx

	inc %rcx
	jmp part_loop_cond

part_loop_done:
	// swap(xs, ii, hi - 1);
	mov %r15, %rdi
	mov %r12, %rsi
	mov %r14, %rdx
	call swap

	// return ii;
	mov %r12, %rax

partition_done:
	leave
	pop %r12
	pop %r13
	pop %r14
	pop %r15
	ret

swap:
	enter $0, $0
	// %rdi --> *xs
	// %rsi --> ii
	// %rdx --> jj

	// if (ii != jj)
	cmp %rsi, %rdx
	je swap_done

	// long tmp = xs[ii]; // tmp --> %r10
	lea (%rdi, %rsi, 8), %r10
	lea (%rdi, %rdx, 8), %r11 // xs[jj] --> %r11


	// xs[ii] = xs[jj];
	mov %r11, (%rdi, %rsi, 8);

	// xs[jj] = tmp;
	mov %r10, (%rdi, %rdx, 8);
swap_done:
	leave
	ret
