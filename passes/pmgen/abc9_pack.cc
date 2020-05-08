/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Clifford Wolf <clifford@clifford.at>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include "kernel/yosys.h"
#include "kernel/sigtools.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

#include "passes/pmgen/abc9_pack_pm.h"

void create_abc9_pack_NNN3(abc9_pack_pm &pm)
{
	auto &st = pm.st_abc9_pack_NNN3;
	log_assert(st.root);
	log_assert(st.root->type == ID($lut));
	log_assert(st.root->getParam(ID::WIDTH) == 3);

	log("  replacing structure rooted at '%s' with $__ABC9_CLUSTER_NNN3 cell.\n", log_id(st.root));

	auto A = st.root->getPort(ID::A);
	auto cell = pm.module->addCell(NEW_ID, ID($__ABC9_CLUSTER_NNN3));
	cell->setParam(ID(B_LUT), st.root->getParam(ID::LUT));
	cell->setPort(ID::Y, st.root->getPort(ID::Y));

	if (st.A0) {
		cell->setPort(ID(A0), st.A0->getPort(ID::A));
		cell->setParam(ID(A0_WIDTH), st.A0->getParam(ID::WIDTH));
		cell->setParam(ID(A0_LUT), st.A0->getParam(ID::LUT));
		pm.autoremove(st.A0);
	}
	else {
		// Add route-through
		cell->setPort(ID(A0), A[0]);
		cell->setPort(ID(A0_LUT), Const::from_string("10"));
		cell->setParam(ID(A0_WIDTH), 1);
	}

	if (st.A1) {
		cell->setPort(ID(A1), st.A1->getPort(ID::A));
		cell->setParam(ID(A1_WIDTH), st.A1->getParam(ID::WIDTH));
		cell->setParam(ID(A1_LUT), st.A1->getParam(ID::LUT));
		pm.autoremove(st.A1);
	}
	else {
		// Add route-through
		cell->setPort(ID(A1), A[1]);
		cell->setPort(ID(A1_LUT), Const::from_string("10"));
		cell->setParam(ID(A1_WIDTH), 1);
	}
	
	if (st.A2) {
		cell->setPort(ID(A2), st.A2->getPort(ID::A));
		cell->setParam(ID(A2_WIDTH), st.A2->getParam(ID::WIDTH));
		cell->setParam(ID(A2_LUT), st.A2->getParam(ID::LUT));
		pm.autoremove(st.A2);
	}
	else {
		// Add route-through
		cell->setPort(ID(A2), A[2]);
		cell->setPort(ID(A2_LUT), Const::from_string("10"));
		cell->setParam(ID(A2_WIDTH), 1);
	}

	pm.autoremove(st.root);
}

struct Abc9PackPass : public Pass {
	Abc9PackPass() : Pass("abc9_pack", "abc9: pack LUT structures") { }
	void help() YS_OVERRIDE
	{
		//   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
		log("\n");
		log("    abc9_pack [selection]\n");
		log("\n");
	}
	void execute(std::vector<std::string> args, RTLIL::Design *design) YS_OVERRIDE
	{
		log_header(design, "Executing ABC9_PACK pass (pack LUT structures).\n");

		size_t argidx;
		for (argidx = 1; argidx < args.size(); argidx++)
		{
			break;
		}
		extra_args(args, argidx, design);

		for (auto module : design->selected_modules()) {
			abc9_pack_pm(module, module->selected_cells()).run_abc9_pack_NNN3(create_abc9_pack_NNN3);
		}
	}
} Abc9PackPass;

PRIVATE_NAMESPACE_END
