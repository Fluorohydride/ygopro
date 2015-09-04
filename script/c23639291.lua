--アグレッシブ・クラウディアン
function c23639291.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c23639291.condition)
	e1:SetTarget(c23639291.target)
	e1:SetOperation(c23639291.operation)
	c:RegisterEffect(e1)
end
function c23639291.condition(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	return eg:GetCount()==1 and tc:IsControler(tp) and tc:GetPreviousControler()==tp and tc:IsReason(REASON_DESTROY)
		and tc:GetReasonEffect() and tc:GetReasonEffect():GetHandler()==tc
end
function c23639291.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc==eg:GetFirst() end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and eg:GetFirst():IsCanBeSpecialSummoned(e,0,tp,false,false) and eg:GetFirst():IsCanBeEffectTarget(e) end
	Duel.SetTargetCard(eg:GetFirst())
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,eg:GetFirst(),1,0,0)
end
function c23639291.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and Duel.SpecialSummonStep(tc,0,tp,tp,false,false,POS_FACEUP_ATTACK) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CANNOT_CHANGE_POS_E)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
		Duel.SpecialSummonComplete()
		tc:AddCounter(0x19,1)
	end
end
