--レスキューラット
function c80200039.initial_effect(c)
	--pendulum summon
	aux.AddPendulumProcedure(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--to Hand
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(80200039,0))
	e2:SetCategory(CATEGORY_TO_HAND)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1,80200039)
	e2:SetCondition(c80200039.addcon)
	e2:SetCost(c80200039.addcost)
	e2:SetTarget(c80200039.addtg)
	e2:SetOperation(c80200039.addop)
	c:RegisterEffect(e2)
	--Special Summon
	local e3=Effect.CreateEffect(c)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EVENT_SUMMON_SUCCESS)
	e3:SetOperation(c80200039.sumsuc)
	c:RegisterEffect(e3)
	--special summon
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(80200039,0))
	e4:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e4:SetType(EFFECT_TYPE_IGNITION)
	e4:SetRange(LOCATION_MZONE)
	e4:SetCost(c80200039.spcost)
	e4:SetCondition(c80200039.spcon)
	e4:SetTarget(c80200039.sptg)
	e4:SetOperation(c80200039.spop)
	c:RegisterEffect(e4)
end
function c80200039.addcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSequence()==6 or e:GetHandler():GetSequence()==7
end
function c80200039.addcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c80200039.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_PENDULUM) and c:IsAbleToHand()
end
function c80200039.filter1(c,g)
	return g:IsExists(Card.IsCode,1,c,c:GetCode())
end
function c80200039.addtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local g=Duel.GetMatchingGroup(c80200039.filter,tp,LOCATION_EXTRA,0,nil)
		return g:IsExists(c80200039.filter1,1,nil,g)
	end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,2,tp,LOCATION_EXTRA)
end
function c80200039.addop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c80200039.filter,tp,LOCATION_EXTRA,0,nil)
	local sg=g:Filter(c80200039.filter1,nil,g)
	if sg:GetCount()==0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local hg=sg:Select(tp,1,1,nil)
	local hc=sg:Filter(Card.IsCode,hg:GetFirst(),hg:GetFirst():GetCode()):GetFirst()
	hg:AddCard(hc)
	Duel.SendtoHand(hg,nil,REASON_EFFECT)
	Duel.ConfirmCards(1-tp,hg)
end
function c80200039.sumsuc(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():RegisterFlagEffect(80200039,RESET_EVENT+0x1fc0000+RESET_PHASE+PHASE_END,0,1)
end
function c80200039.spcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(80200039)~=0
end
function c80200039.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c80200039.spfilter(c,e,tp)
	return c:IsType(TYPE_PENDULUM) and c:IsLevelBelow(5) and c:IsFaceup() and 
	Duel.IsExistingMatchingCard(c80200039.spfilter1,tp,LOCATION_DECK,0,2,nil,c:GetCode(),e,tp)
end
function c80200039.spfilter1(c,code,e,tp)
	return c:GetCode()==code and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c80200039.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		return Duel.IsExistingMatchingCard(c80200039.spfilter,tp,LOCATION_EXTRA,0,1,nil,e,tp)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0 
	end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,2,tp,LOCATION_DECK)
end
function c80200039.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<2 then return end
	local g=Duel.GetMatchingGroup(c80200039.spfilter,tp,LOCATION_EXTRA,0,nil,e,tp)
	if g:GetCount()==0 then return end
	local tc=g:Select(tp,1,1,nil):GetFirst()
	local code=tc:GetCode()
	local dg=Duel.GetMatchingGroup(c80200039.spfilter1,tp,LOCATION_DECK,0,nil,code,e,tp)
	if dg:GetCount()>=1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=dg:Select(tp,1,1,nil)
		local tc1=sg:GetFirst()
		dg:RemoveCard(tc1)
		local tc2=dg:Filter(Card.IsCode,nil,tc1:GetCode()):GetFirst()
		Duel.SpecialSummonStep(tc1,0,tp,tp,false,false,POS_FACEUP)
		Duel.SpecialSummonStep(tc2,0,tp,tp,false,false,POS_FACEUP)
		tc1:RegisterFlagEffect(80200039,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
		tc2:RegisterFlagEffect(80200039,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
		Duel.SpecialSummonComplete()
		sg:AddCard(tc2)
		sg:KeepAlive()
		local c=e:GetHandler()
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EVENT_PHASE+PHASE_END)
		e1:SetReset(RESET_PHASE+PHASE_END)
		e1:SetCountLimit(1)
		e1:SetLabelObject(sg)
		e1:SetOperation(c80200039.desop)
		Duel.RegisterEffect(e1,tp)
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_DISABLE)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		tc1:RegisterEffect(e2)
		local e3=e2:Clone()
		tc2:RegisterEffect(e3)
		local e4=Effect.CreateEffect(c)
		e4:SetType(EFFECT_TYPE_SINGLE)
		e4:SetCode(EFFECT_DISABLE_EFFECT)
		e4:SetReset(RESET_EVENT+0x1fe0000)
		tc1:RegisterEffect(e4)
		local e5=e4:Clone()
		tc2:RegisterEffect(e5)
	end
end
function c80200039.desfilter(c)
	return c:GetFlagEffect(80200039)>0
end
function c80200039.desop(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	local tg=g:Filter(c80200039.desfilter,nil)
	g:DeleteGroup()
	Duel.Destroy(tg,REASON_EFFECT)
end
