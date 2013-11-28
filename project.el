(defvar rootDir (file-name-directory (or load-file-name buffer-file-name)))

(defun premake ()
  (interactive)
  (save-window-excursion
    (shell-command (concat "(cd " rootDir " && premake4 gmake && cd -)" )))
  )

(setq compile-command (concat "(cd " rootDir " && make -k -j && cd -)"))

(defun runProgram ()
  (interactive)
  (save-window-excursion
    (shell-command (concat "(cd " rootDir " && ./run && cd -)" )))
  )

(global-set-key (kbd "<f7>") 'premake)
(global-set-key (kbd "C-<f5>") 'compile)
(global-set-key (kbd "<f5>") 'recompile)
(global-set-key (kbd "<f6>") 'runProgram)
