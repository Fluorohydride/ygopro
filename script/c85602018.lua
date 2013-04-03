--遺言状
function c85602018.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetOperation(c85602018.operation)
	c:RegisterEffect(e1)
end
function c85602018.operation(e,tp,eg,ep,ev,re,r,rp)
	--spsummon
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c85602018.check)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
	local e2=Effect.CreateEffect(e:GetHandler())
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_SUMMON_SUCCESS)
	e2:SetCondition(c85602018.spcon)
	e2:SetOperation(c85602018.spop)
	e2:SetLabelObject(e1)
	e2:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e2,tp)
	local e3=e2:Clone()
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	e3:SetCondition(c85602018.spcon2)
	Duel.RegisterEffect(e3,tp)
	local e4=e2:Clone()
	e4:SetCode(EVENT_CHAIN_END)
	Duel.RegisterEffect(e4,tp)
	local e5=e2:Clone()
	e5:SetCode(EVENT_ATTACK_ANNOUNCE)
	Duel.RegisterEffect(e5,tp)
	local e6=e2:Clone()
	e6:SetCode(EVENT_DAMAGE_STEP_END)
	Duel.RegisterEffect(e6,tp)
	local e7=e2:Clone()
	e7:SetCode(EVENT_PHASE+PHASE_END)
	Duel.RegisterEffect(e7,tp)
end
function c85602018.cfilter(c,tp)
	return c:IsControler(tp) and c:GetPreviousControler()==tp and c:IsPreviousLocation(LOCATION_MZONE)
end
function c85602018.check(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==2 then return end
	if eg:IsExists(c85602018.cfilter,1,nil,tp) then
		e:SetLabel(1)
	end
end
function c85602018.spfilter(c,e,tp)
	return c:IsAttackBelow(1500) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c85602018.spcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetLabelObject():GetLabel()==1
end
function c85602018.spcon2(e,tp,eg,ep,ev,re,r,rp)
	return e:GetLabelObject():GetLabel()==1 and Duel.GetCurrentChain()==0
end
function c85602018.spop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabelObject():GetLabel()~=1 then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c85602018.spfilter,tp,LOCATION_DECK,0,1,nil,e,tp)
		and Duel.SelectYesNo(tp,aux.Stringid(85602018,0)) then
		e:GetLabelObject():SetLabel(2)
		Duel.Hint(HINT_CARD,0,85602018)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectMatchingCard(tp,c85602018.spfilter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	else
		e:GetLabelObject():SetLabel(0)
	end
end
