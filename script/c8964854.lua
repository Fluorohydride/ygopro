--コンビネーション・アタック
function c8964854.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCondition(c8964854.condition)
	e1:SetTarget(c8964854.target)
	e1:SetOperation(c8964854.operation)
	c:RegisterEffect(e1)
end
function c8964854.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()==PHASE_BATTLE
end
function c8964854.filter(c,e,tp)
	return c:GetAttackAnnouncedCount()>0 and Duel.IsExistingMatchingCard(c8964854.eqfilter,tp,LOCATION_SZONE,0,1,nil,e,tp,c)
end
function c8964854.eqfilter(c,e,tp,ec)
	return c:IsStatus(STATUS_UNION) and c:GetEquipTarget()==ec and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c8964854.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c8964854.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c8964854.filter,tp,LOCATION_MZONE,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c8964854.filter,tp,LOCATION_MZONE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_SZONE)
end
function c8964854.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) and Duel.GetLocationCount(tp,LOCATION_MZONE)>0 then
		local g=Duel.GetMatchingGroup(c8964854.eqfilter,tp,LOCATION_SZONE,0,nil,e,tp,tc)
		if Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)>0 then
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_EXTRA_ATTACK)
			e1:SetValue(tc:GetAttackAnnouncedCount())
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			tc:RegisterEffect(e1)
		end
	end
end
