--レスキューラット
function c50485594.initial_effect(c)
	--pendulum summon
	aux.AddPendulumProcedure(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--to hand
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(50485594,0))
	e2:SetCategory(CATEGORY_TOHAND)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_PZONE)
	e2:SetCountLimit(1,50485594+EFFECT_COUNT_CODE_DUEL)
	e2:SetCost(c50485594.thcost)
	e2:SetTarget(c50485594.thtg)
	e2:SetOperation(c50485594.thop)
	c:RegisterEffect(e2)
	--special summon
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EVENT_SUMMON_SUCCESS)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e3:SetOperation(c50485594.regop)
	c:RegisterEffect(e3)
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(50485594,1))
	e4:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e4:SetType(EFFECT_TYPE_IGNITION)
	e4:SetRange(LOCATION_MZONE)
	e4:SetCost(c50485594.spcost)
	e4:SetCondition(c50485594.spcon)
	e4:SetTarget(c50485594.sptg)
	e4:SetOperation(c50485594.spop)
	c:RegisterEffect(e4)
end
function c50485594.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c50485594.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_PENDULUM) and c:IsAbleToHand()
end
function c50485594.filter2(c,g)
	return g:IsExists(Card.IsCode,1,c,c:GetCode())
end
function c50485594.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local g=Duel.GetMatchingGroup(c50485594.filter,tp,LOCATION_EXTRA,0,nil)
		return g:IsExists(c50485594.filter2,1,nil,g)
	end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,2,tp,LOCATION_EXTRA)
end
function c50485594.thop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c50485594.filter,tp,LOCATION_EXTRA,0,nil)
	local sg=g:Filter(c50485594.filter2,nil,g)
	if sg:GetCount()==0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local hg1=sg:Select(tp,1,1,nil)
	local hg2=sg:Filter(Card.IsCode,hg1:GetFirst(),hg1:GetFirst():GetCode())
	hg1:AddCard(hg2:GetFirst())
	Duel.SendtoHand(hg1,nil,REASON_EFFECT)
	Duel.ConfirmCards(1-tp,hg1)
end
function c50485594.regop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():RegisterFlagEffect(50485594,RESET_EVENT+0x1fc0000+RESET_PHASE+PHASE_END,0,1)
end
function c50485594.spcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(50485594)~=0
end
function c50485594.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c50485594.spfilter(c,e,tp)
	return c:IsFaceup() and c:IsType(TYPE_PENDULUM) and c:IsLevelBelow(5)
		and Duel.IsExistingMatchingCard(c50485594.spfilter2,tp,LOCATION_DECK,0,2,nil,c:GetCode(),e,tp)
end
function c50485594.spfilter2(c,code,e,tp)
	return c:IsCode(code) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c50485594.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>1
		and Duel.IsExistingMatchingCard(c50485594.spfilter,tp,LOCATION_EXTRA,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,2,tp,LOCATION_DECK)
end
function c50485594.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<2 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g=Duel.SelectMatchingCard(tp,c50485594.spfilter,tp,LOCATION_EXTRA,0,1,1,nil,e,tp)
	if g:GetCount()==0 then return end
	local code=g:GetFirst():GetCode()
	local dg=Duel.GetMatchingGroup(c50485594.spfilter2,tp,LOCATION_DECK,0,nil,code,e,tp)
	if dg:GetCount()>1 then
		local c=e:GetHandler()
		local fid=c:GetFieldID()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg1=dg:Select(tp,1,1,nil)
		local tc1=sg1:GetFirst()
		local sg2=dg:Filter(Card.IsCode,tc1,tc1:GetCode())
		local tc2=sg2:GetFirst()
		Duel.SpecialSummonStep(tc1,0,tp,tp,false,false,POS_FACEUP)
		Duel.SpecialSummonStep(tc2,0,tp,tp,false,false,POS_FACEUP)
		tc1:RegisterFlagEffect(50485594,RESET_EVENT+0x1fe0000,0,1,fid)
		tc2:RegisterFlagEffect(50485594,RESET_EVENT+0x1fe0000,0,1,fid)
		Duel.SpecialSummonComplete()
		sg1:Merge(sg2)
		sg1:KeepAlive()
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EVENT_PHASE+PHASE_END)
		e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
		e1:SetCountLimit(1)
		e1:SetLabel(fid)
		e1:SetLabelObject(sg1)
		e1:SetCondition(c50485594.descon)
		e1:SetOperation(c50485594.desop)
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
function c50485594.desfilter(c,fid)
	return c:GetFlagEffectLabel(50485594)==fid
end
function c50485594.descon(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	if not g:IsExists(c50485594.desfilter,1,nil,e:GetLabel()) then
		g:DeleteGroup()
		e:Reset()
		return false
	else return true end
end
function c50485594.desop(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	local tg=g:Filter(c50485594.desfilter,nil,e:GetLabel())
	Duel.Destroy(tg,REASON_EFFECT)
end
