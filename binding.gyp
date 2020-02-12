{
	"targets": [
		{
			"target_name": "fontemp",
			"conditions": [
				['OS=="win"',
					{
						"sources": [ "./src/fontemp.cc" ],
						"include_dirs": [
							"<!(node -e \"require('nan')\")"
						]
					}
				],
				['OS!="win"', {}],
			]
		},

		# {
		# 	"target_name": "action_after_build",
		# 	"type": "none",
		# 	"copies": [
		# 		{
		# 			"files": [ "<(PRODUCT_DIR)" ],
		# 			"destination": "dist"
		# 		}
		# 	]
		# }
	],
}
