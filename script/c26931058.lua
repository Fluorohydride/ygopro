--フォーメーション・ユニオン
function c26931058.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c26931058.eftg)
	e1:SetOperation(c26931058.efop)
	c:RegisterEffect(e1)
end
function c26931058.filter1(c,tp)
	return c:IsFaceup() and c:IsType(TYPE_UNION)
		and Duel.IsExistingTarget(c26931058.filter2,tp,LOCATION_MZONE,0,1,c,c)
end
function c26931058.filter2(c,ec)
	return c:IsFaceup() and ec:CheckEquipTarget(c)
end
function c26931058.filter3(c,e,tp)
	return c:IsFaceup() and c:IsStatus(STATUS_UNION) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c26931058.eftg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	local b1=Duel.IsExistingMatchingCard(c26931058.filter1,tp,LOCATION_MZONE,0,1,nil,tp) and Duel.GetLocationCount(tp,LOCATION_SZONE)>0
	local b2=Duel.IsExistingMatchingCard(c26931058.filter3,tp,LOCATION_SZONE,0,1,nil,e,tp) and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
	if chk==0 then return b1 or b2 end
	local op=0
	if b1 and b2 then
		op=Duel.SelectOption(tp,aux.Stringid(26931058,0),aux.Stringid(26931058,1))
	elseif b1 then
		op=Duel.SelectOption(tp,aux.Stringid(26931058,0))
	else op=Duel.SelectOption(tp,aux.Stringid(26931058,1))+1 end
	e:SetLabel(op)
	if op==0 then
		e:SetCategory(CATEGORY_EQUIP)
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(26931058,2))
		local g1=Duel.SelectTarget(tp,c26931058.filter1,tp,LOCATION_MZONE,0,1,1,nil,tp)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
		local g2=Duel.SelectTarget(tp,c26931058.filter2,tp,LOCATION_MZONE,0,1,1,g1:GetFirst(),g1:GetFirst())
		e:SetLabelObject(g1:GetFirst())
	else
		e:SetCategory(CATEGORY_SPECIAL_SUMMON)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectTarget(tp,c26931058.filter3,tp,LOCATION_SZONE,0,1,1,nil,e,tp)
		Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
	end
end
function c26931058.efop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 then
		local tc1=e:GetLabelObject()
		local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
		local tc2=g:GetFirst()
		if tc1==tc2 then tc2=g:GetNext() end
		if tc1:IsFaceup() and tc2:IsFaceup() and tc1:IsRelateToEffect(e) and tc2:IsRelateToEffect(e) and Duel.Equip(tp,tc1,tc2,false) then
			tc1:SetStatus(STATUS_UNION,true)
		end
	else
		local tc=Duel.GetFirstTarget()
		if tc and tc:IsRelateToEffect(e) then
			Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP_ATTACK)
		end
	end
end
