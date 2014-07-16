--エクシーズ・ユニバース
function c11109820.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(11109820,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c11109820.target)
	e1:SetOperation(c11109820.operation)
	c:RegisterEffect(e1)
end
function c11109820.filter1(c,e,tp,ft)
	if c:IsControler(tp) then ft=ft+1 end
	return c:IsFaceup() and c:IsType(TYPE_XYZ)
		and Duel.IsExistingTarget(c11109820.filter2,tp,LOCATION_MZONE,LOCATION_MZONE,1,c,e,tp,c:GetRank(),ft)
end
function c11109820.filter2(c,e,tp,rk,ft)
	if c:IsControler(tp) then ft=ft+1 end
	return c:IsFaceup() and c:IsType(TYPE_XYZ)
		and ft>0 and Duel.IsExistingMatchingCard(c11109820.spfilter,tp,LOCATION_EXTRA,0,1,nil,e,tp,rk+c:GetRank())
end
function c11109820.spfilter(c,e,tp,rk)
	local crk=c:GetRank()
	return (crk==rk or crk==rk-1) and not c:IsSetCard(0x48) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c11109820.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if chk==0 then return Duel.IsExistingTarget(c11109820.filter1,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,e,tp,ft) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g1=Duel.SelectTarget(tp,c11109820.filter1,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil,e,tp,ft)
	local tc=g1:GetFirst()
	if tc:IsControler(tp) then ft=ft+1 end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g2=Duel.SelectTarget(tp,c11109820.filter2,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,tc,e,tp,tc:GetRank(),ft)
	g1:Merge(g2)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,g1,2,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c11109820.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tc1=g:GetFirst()
	local tc2=g:GetNext()
	if not tc1:IsRelateToEffect(e) or not tc2:IsRelateToEffect(e) then return end
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if tc1:IsControler(tp) then ft=ft+1 end
	if tc2:IsControler(tp) then ft=ft+1 end
	if ft<=0 then return end
	local sg=Duel.GetMatchingGroup(c11109820.spfilter,tp,LOCATION_EXTRA,0,nil,e,tp,tc1:GetRank()+tc2:GetRank())
	if sg:GetCount()==0 then return end
	Duel.SendtoGrave(g,REASON_EFFECT)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local ssg=sg:Select(tp,1,1,nil)
	local sc=ssg:GetFirst()
	if sc then
		Duel.SpecialSummon(sc,0,tp,tp,false,false,POS_FACEUP)
		if c:IsRelateToEffect(e) then
			c:CancelToGrave()
			Duel.Overlay(sc,Group.FromCards(c))
		end
	end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CHANGE_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(0,1)
	e1:SetValue(0)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
