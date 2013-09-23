<?php
/**
 * This file is part of OpenMediaVault.
 *
 * @license		http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author		Volker Theile <volker.theile@openmediavault.org>
 * @author		Aaron Murray <aaron@omv-extras.org>
 * @author		Ian Moore <imooreyahoo@gmail.com>
 * @author		Marcel Beck <marcel.beck@mbeck.org>
 * @copyright	Copyright (c) 2009-2013 Volker Theile 
 * @copyright	Copyright (c) 2011 Ian Moore
 * @copyright	Copyright (c) 2012 Marcel Beck
 * @copyright	Copyright (c) 2013 Aaron Murray
 *
 * OpenMediaVault is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * OpenMediaVault is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenMediaVault. If not, see <http://www.gnu.org/licenses/>.
 */
require_once("openmediavault/object.inc");
require_once("openmediavault/error.inc");
require_once("openmediavault/module.inc");
require_once("openmediavault/rpcservice.inc");

class OMVRpcServiceProcessList extends OMVRpcServiceAbstract {
	/**
	 * Get the RPC service name.
	 */
	public function getName() {
		return "ProcessList";
	}

	/**
	 * Initialize the RPC service.
	 */
	public function initialize() {
		$this->registerMethod("getStatus");
	}

	/**
	 * Get the status of the modules that implement the
	 * OMVIModuleServiceStatus interface.
	 * @param params The method parameters.
	 * @param context The context of the caller.
	 * @return An array containing the module statistics.
	 * @throw E_EXEC_FAILED
	 */
	function getStatus($params, $context) {
		// Validate the RPC caller context.
		$this->validateMethodContext($context, array(
			  "role" => OMV_ROLE_ADMINISTRATOR
		  ));
		
		// Get ps output
		$cmd = 'sudo ps axww -o pid,user,%cpu,%mem,stat,args 2>&1';
		OMVUtil::exec($cmd, $output, $result);
		
		if ($result !== 0)
			throw new OMVException(OMVErrorMsg::E_EXEC_FAILED, $cmd, implode("\n", $output));

		// Get column names
		$cols = array_map(function($a)
		{
			return preg_replace("/[^A-Z]+/", '', $a);
		}, preg_split('/\s+/', trim(array_shift($output))));

		$split_limit = count($cols);

		$objects = array();
		foreach ($output as $line)
		{
			if (trim($line) == '')
			{
				continue;
			}
			$objects[] = array_combine($cols, preg_split('/\s+/', trim($line), $split_limit));
		}

		return array(
			"total" => count($objects),
			"data" => $objects
		);
	}
}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance();
$rpcServiceMgr->registerService(new OMVRpcServiceProcessList());
?>
