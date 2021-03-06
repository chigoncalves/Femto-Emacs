;; FemtoEmacs init.lsp, should be placed in the users HOME directory together with "r5rs.scm"
;; For now other examples of extensions from the samples directory should be placed
;; in the HOME directory. For example killring.scm and buffmenu.scm
;;
;;
;; Compatibility with SICP
(define (home f)
   (string (os.getenv "HOME") "/" f))

(load (home "r5rs.scm"))

;;
;; The function read-string is necessary for reading a Lisp list from a string. 
;; When Lisp retrieves a text such as "(2016 8 31)" the text comes out as a string. 
;; On the other hand, (read-string "(2016 8 31)" ) reads a list from this string in the 
;; same way as it would read a list from a file.
;;
(define (read-string str)
    (let ( (port (open-input-string str)))
      (begin0 (read port)
	      (close-input-port port)) ))


;;
;; returns weekday as a string
;;
(define (weekday n)
    (cond ( (equal? n 0) "Sunday")
	  ( (equal? n 1) "Monday")
	  ( (equal? n 2) "Tuesday")
	  ( (equal? n 3) "Wednesday")
	  ( (equal? n 4) "Thursday")
	  ( (equal? n 5) "Friday")
	  ( (equal? n 6) "Saturday")
	  (else "unknown")))



;;
;; returns the day for the specific date
;; (what-day '(2016 9 3))
(define (what-day ymd)
    (let* ( (y (car ymd))
            (m (cadr ymd))
            (d (caddr ymd))
            (ax (quotient (- 14 m) 12))
	    (mm (+ m (* 12 ax) -2))
	    (yy (- y ax))
	    (c (- (quotient yy 100)))
	    (ly (quotient yy 4)) )
      (weekday (mod (+ yy
		    (quotient (- (* 13 mm) 1) 5)
		    ly
                    c
		    (quotient yy 400) d) 7)) ))

;; call kill-region and return it as a string
(define (cut-region)
  (kill-region)
  (get-clipboard))

;; bound to C-x C-u
(define (upcase-region)
   (kill-region)
   (set-clipboard (string.map char.upcase (get-clipboard)))
   (yank)
   (clear-message-line))

;; bound to C-x C-l
(define (downcase-region)
   (kill-region)
   (set-clipboard (string.map char.downcase (get-clipboard)))
   (yank)
   (clear-message-line))

;; Shortcut definitions
(define (keyboard key) 
   (cond
      ( (equal? key "C-c z")
        (insert (what-day (read-string (cut-region))) ))
      ( (equal? key "C-c a") 
        (insert "<p> </p>")
        (backward-char 5))

      ( (equal? key "C-c b")
        (beginning-of-line) 
        (insert "<h1> </h2>")
        (beginning-of-line)
        (forward-char 4))

      ( (equal? key "C-c c")
        (end-of-line)
        (insert "<p>-")
        (insert (get-clipboard))
        (insert "-</p>"))

      ( (equal? key "C-c i")
        (if (not (top-level-bound? 'kill-ring-menu))
          (load (home "killring.scm")))
          (insert-kill-ring))

      ( (equal? key "C-c k")
        (if (not (top-level-bound? 'kill-ring-menu))
          (load (home "killring.scm")))
          (kill-ring-menu))

      ( (equal? key "C-c m")
        (if (not (top-level-bound? 'bufm-stop))
          (load (home "bufmenu.scm")))
          (buffer-menu))

      ( (equal? key "C-x C-u")
          (upcase-region))

      ( (equal? key "C-x C-l")
          (downcase-region))

      (else (insert key)) ))

;;
;; Initialise the kill-ring to empty
;;
(define kill-ring ())

;;
;; append to the kill-ring on every kill
;; 
(define (kill-hook bufn)
        (set! kill-ring (cons (get-clipboard) kill-ring)))

;; define syntax highlighting for scheme files
(newlanguage ".scm" ";" "#|" "|#")
(keyword "define")
(keyword "cond")
(keyword "else")

;; define syntax hughlihgting for lisp files
(newlanguage ".lsp" ";" "#|" "|#")
(keyword "define")
(keyword "cond")
(keyword "else")

;; define syntax hughlighting for C code
(newlanguage ".c" "//" "/*" "*/")
(keyword "auto")
(keyword "break")
(keyword "case")
(keyword "char")
(keyword "const")
(keyword "continue")
(keyword "default")
(keyword "do")
(keyword "double")
(keyword "else")
(keyword "enum")
(keyword "extern")
(keyword "float")
(keyword "for")
(keyword "goto")
(keyword "if")
(keyword "int")
(keyword "long")
(keyword "register")
(keyword "return")
(keyword "short")
(keyword "signed")
(keyword "sizeof")
(keyword "static")
(keyword "struct")
(keyword "switch")
(keyword "typedef")
(keyword "union")
(keyword "unsigned")
(keyword "void")
(keyword "volatile")
(keyword "while")

