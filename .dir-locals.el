((nil . ((eval . (progn
		   (require 'format-spec)
		   (require 'projectile)
		   (require 'company-clang)
		   (require 'flycheck)
                   (setq-local company-clang-arguments (delete-dups
						  (append
						   company-clang-arguments
						   (list
						    (concat "-I" (projectile-project-root) ".")
						    (concat "-I" (projectile-project-root) "core/fntest")
						    (concat "-I" (projectile-project-root) "windows")
						    "-D PUBNUB_CALLBACK_API"
						    "-D PUBNUB_THREADSAFE"
						    "-D PUBNUB_PROXY_API=1"
						    ))))
                   (setq-local flycheck-clang-include-path (delete-dups
						  (append
						   flycheck-clang-include-path
						   (list
						    (concat (projectile-project-root) ".")
						    (concat (projectile-project-root) "windows")
						    ))))
                   (setq-local flycheck-clang-definitions (delete-dups
						  (append
						   flycheck-clang-definitions
						   (list
						    "PUBNUB_CALLBACK_API"
						    "PUBNUB_THREADSAFE"
						    "PUBNUB_PROXY_API=1"
						    ))))
                   (setq-local flycheck-cppcheck-include-path (delete-dups
						  (append
						   flycheck-cppcheck-include-path
						   (list
						    (concat (projectile-project-root) ".")
						    (concat (projectile-project-root) "core/fntest")
						    (concat (projectile-project-root) "windows")
						    ))))
		   )))))
