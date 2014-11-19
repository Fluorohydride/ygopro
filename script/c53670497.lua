--復活の聖刻印
function c53670497.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetTarget(c53670497.target1)
	e1:SetOperation(c53670497.activate)
	c:RegisterEffect(e1)
	--instant
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(53670497,1))
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetHintTiming(0,TIMING_END_PHASE)
	e2:SetLabel(1)
	e2:SetTarget(c53670497.target2)
	e2:SetOperation(c53670497.activate)
	c:RegisterEffect(e2)
	--spsummmon
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(53670497,2))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_TRIGGER_F+EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetCode(EVENT_TO_GRAVE)
	e3:SetCondition(c53670497.spcon)
	e3:SetTarget(c53670497.sptg)
	e3:SetOperation(c53670497.spop)
	c:RegisterEffect(e3)
end
function c53670497.filter1(c)
	return c:IsSetCard(0x69) and c:IsType(TYPE_MONSTER) and c:IsAbleToGrave()
end
function c53670497.filter2(c)
	return c:IsFaceup() and c:IsSetCard(0x69) and c:IsType(TYPE_MONSTER)
end
function c53670497.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	e:SetLabel(0)
	local turnp=Duel.GetTurnPlayer()
	if ((turnp~=tp and Duel.IsExistingMatchingCard(c53670497.filter1,tp,LOCATION_DECK,0,1,nil))
		or (turnp==tp and Duel.IsExistingMatchingCard(c53670497.filter2,tp,LOCATION_REMOVED,0,1,nil)))
		and Duel.SelectYesNo(tp,aux.Stringid(53670497,0)) then
		if turnp==tp then
			Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_REMOVED)
		else
			Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
		end
		e:SetLabel(1)
		e:GetHandler():RegisterFlagEffect(53670497,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	end
end
function c53670497.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	local turnp=Duel.GetTurnPlayer()
	if chk==0 then return e:GetHandler():GetFlagEffect(53670497)==0
		and ((turnp~=tp and Duel.IsExistingMatchingCard(c53670497.filter1,tp,LOCATION_DECK,0,1,nil))
		or (turnp==tp and Duel.IsExistingMatchingCard(c53670497.filter2,tp,LOCATION_REMOVED,0,1,nil))) end
	if turnp==tp then
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_REMOVED)
	else
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
	end
	e:GetHandler():RegisterFlagEffect(53670497,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c53670497.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if e:GetLabel()~=1 or not c:IsRelateToEffect(e) then return end
	local turnp=Duel.GetTurnPlayer()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	if turnp~=tp then
		local g=Duel.SelectMatchingCard(tp,c53670497.filter1,tp,LOCATION_DECK,0,1,1,nil)
		Duel.SendtoGrave(g,REASON_EFFECT)
	else
		local g=Duel.SelectMatchingCard(tp,c53670497.filter2,tp,LOCATION_REMOVED,0,1,1,nil)
		Duel.SendtoGrave(g,REASON_EFFECT+REASON_RETURN)
	end
end
function c53670497.spcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:IsPreviousPosition(POS_FACEUP) and c:IsPreviousLocation(LOCATION_ONFIELD)
end
function c53670497.filter(c,e,tp)
	return c:IsSetCard(0x69) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c53670497.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c53670497.filter(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c53670497.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c53670497.filter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c53670497.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
