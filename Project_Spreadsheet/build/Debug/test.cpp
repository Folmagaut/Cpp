void TestCircularDependencies2() {
    {
        auto sheet = CreateSheet();
        bool caught = false;
        try {
            sheet->SetCell("A1"_pos, "=A1");
        }
        catch (const CircularDependencyException&) {
            caught = true;
        }
        ASSERT(caught);
        ASSERT(sheet->GetCell("A1"_pos) == nullptr);
    }

    {
        auto sheet = CreateSheet();
        sheet->SetCell("A1"_pos, "=B1");
        bool caught = false;
        try {
            sheet->SetCell("B1"_pos, "=A1");
        }
        catch (const CircularDependencyException&) {
            caught = true;
        }
        ASSERT(caught);
        ASSERT_EQUAL(sheet->GetCell("A1"_pos)->GetText(), "=B1");
        ASSERT(sheet->GetCell("B1"_pos) != nullptr);
    }

    {
        auto sheet = CreateSheet();
        sheet->SetCell("A1"_pos, "=B1");
        sheet->SetCell("B1"_pos, "=C1");
        bool caught = false;
        try {
            sheet->SetCell("C1"_pos, "=A1");
        }
        catch (const CircularDependencyException&) {
            caught = true;
        }
        ASSERT(caught);
        ASSERT_EQUAL(sheet->GetCell("A1"_pos)->GetText(), "=B1");
        ASSERT_EQUAL(sheet->GetCell("B1"_pos)->GetText(), "=C1");
        ASSERT(sheet->GetCell("C1"_pos) != nullptr);
    }

    // не должно быть ложного срабатывания
    {
        auto sheet = CreateSheet();
        sheet->SetCell("A1"_pos, "=B1+C1");
        sheet->SetCell("B1"_pos, "1");
        sheet->SetCell("C1"_pos, "=D1");
        sheet->SetCell("D1"_pos, "2");
        try {
            (sheet->SetCell("E1"_pos, "=A1+B1"));
        }
        catch (CircularDependencyException) {
            ASSERT(false)
        }

        ASSERT_EQUAL(std::get<double>(sheet->GetCell("E1"_pos)->GetValue()), 4.0);
    }

    // Цикл через несколько уровней (A1 -> B1 -> C1 -> D1 -> B1)
    {
        auto sheet = CreateSheet();
        sheet->SetCell("A1"_pos, "=B1");
        sheet->SetCell("B1"_pos, "=C1");
        sheet->SetCell("C1"_pos, "=D1");
        bool caught = false;
        try {
            sheet->SetCell("D1"_pos, "=B1");
        }
        catch (const CircularDependencyException&) {
            caught = true;
        }
        ASSERT(caught);
        ASSERT_EQUAL(sheet->GetCell("A1"_pos)->GetText(), "=B1");
        ASSERT_EQUAL(sheet->GetCell("B1"_pos)->GetText(), "=C1");
        ASSERT_EQUAL(sheet->GetCell("C1"_pos)->GetText(), "=D1");
        ASSERT(sheet->GetCell("D1"_pos) != nullptr);
    }

    {
        auto sheet = CreateSheet();
        sheet->SetCell("A1"_pos, "=B1");
        sheet->SetCell("B1"_pos, "10");
        sheet->SetCell("C1"_pos, "=A1+B1");

        bool caught = false;
        try {
            sheet->SetCell("B1"_pos, "=C1");
        }
        catch (const CircularDependencyException&) {
            caught = true;
        }
        ASSERT(caught);
        ASSERT_EQUAL(sheet->GetCell("B1"_pos)->GetText(), "10");
    }

    {
        auto sheet = CreateSheet();
        sheet->SetCell("A1"_pos, "=B1+C1");
        sheet->SetCell("B1"_pos, "=D1");
        sheet->SetCell("C1"_pos, "=E1");

        bool caught = false;
        try {
            sheet->SetCell("E1"_pos, "=A1");
        }
        catch (const CircularDependencyException&) {
            caught = true;
        }
        ASSERT(caught);
        ASSERT(sheet->GetCell("E1"_pos) != nullptr);
    }

    // Очень длинная цепочка без цикла (не должно быть ложного срабатывания)
    {
        auto sheet = CreateSheet();
        sheet->SetCell("A1"_pos, "=B1");
        sheet->SetCell("B1"_pos, "=C1");
        sheet->SetCell("C1"_pos, "=D1");
        sheet->SetCell("D1"_pos, "=E1");
        sheet->SetCell("E1"_pos, "=F1");
        sheet->SetCell("F1"_pos, "42");
        try {
            (sheet->SetCell("G1"_pos, "=A1"));
        }
        catch (CircularDependencyException) {
            ASSERT(false)
        }

        ASSERT_EQUAL(std::get<double>(sheet->GetCell("G1"_pos)->GetValue()), 42.0);
    }

    {
        auto sheet = CreateSheet();
        sheet->SetCell("A1"_pos, "=B1");
        bool caught = false;
        try {
            sheet->SetCell("B1"_pos, "=C1");
            sheet->SetCell("C1"_pos, "=A1");
        }
        catch (const CircularDependencyException&) {
            caught = true;
        }
        ASSERT(caught);
        ASSERT_EQUAL(sheet->GetCell("A1"_pos)->GetText(), "=B1");
        ASSERT(sheet->GetCell("C1"_pos) != nullptr);
    }

    {
        auto sheet = CreateSheet();
        sheet->SetCell("A1"_pos, "=B1");
        sheet->SetCell("C1"_pos, "=B1");
        sheet->SetCell("D1"_pos, "=B1");
        sheet->SetCell("B1"_pos, "10");

        bool caught = false;
        try {
            sheet->SetCell("B1"_pos, "=A1");
        }
        catch (const CircularDependencyException&) {
            caught = true;
        }
        ASSERT(caught);
    }

    {
        auto sheet = CreateSheet();
        sheet->SetCell("A1"_pos, "=B1");
        sheet->SetCell("B1"_pos, "=C1");
        sheet->SetCell("C1"_pos, "10");

        bool caught = false;
        try {
            sheet->SetCell("C1"_pos, "=A1");
        }
        catch (const CircularDependencyException&) {
            caught = true;
        }
        ASSERT(caught);
        ASSERT_EQUAL(sheet->GetCell("C1"_pos)->GetText(), "10");
    }

    {
        auto sheet = CreateSheet();
        sheet->SetCell("A1"_pos, "=B1+C1");
        sheet->SetCell("B1"_pos, "=D1+E1");
        sheet->SetCell("C1"_pos, "=F1+G1");
        sheet->SetCell("D1"_pos, "1");
        sheet->SetCell("E1"_pos, "2");
        sheet->SetCell("F1"_pos, "3");
        sheet->SetCell("G1"_pos, "4");
        try {
            (sheet->SetCell("H1"_pos, "=A1+B1+C1"));
        }
        catch (CircularDependencyException) {
            ASSERT(false)
        }

    }
}