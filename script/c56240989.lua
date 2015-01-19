--エヴォルド・カシネリア
function c56240989.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_BATTLE_DESTROYING)
	e1:SetOperation(c56240989.regop)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(56240989,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_PHASE+PHASE_BATTLE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c56240989.spcon)
	e2:SetCost(c56240989.spcost)
	e2:SetTarget(c56240989.sptg)
	e2:SetOperation(c56240989.spop)
	c:RegisterEffect(e2)
end
function c56240989.regop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():RegisterFlagEffect(56240989,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c56240989.spcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(56240989)~=0
end
function c56240989.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c56240989.filter(c,e,tp)
	return c:IsLevelBelow(6) and c:IsRace(RACE_DINOSAUR) and c:IsAttribute(ATTRIBUTE_FIRE)
		and c:IsCanBeSpecialSummoned(e,154,tp,false,false)
end
function c56240989.filter2(c,g)
	return g:IsExists(Card.IsCode,1,c,c:GetCode())
end
function c56240989.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local g=Duel.GetMatchingGroup(c56240989.filter,tp,LOCATION_DECK,0,nil,e,tp)
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and g:IsExists(c56240989.filter2,1,nil,g)
	end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,2,tp,LOCATION_DECK)
end
function c56240989.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<2 then return end
	local c=e:GetHandler()
	local g=Duel.GetMatchingGroup(c56240989.filter,tp,LOCATION_DECK,0,nil,e,tp)
	local dg=g:Filter(c56240989.filter2,nil,g)
	if dg:GetCount()>=1 then
		local fid=c:GetFieldID()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=dg:Select(tp,1,1,nil)
		local tc1=sg:GetFirst()
		local tc2=dg:Filter(Card.IsCode,tc1,tc1:GetCode()):GetFirst()
		Duel.SpecialSummonStep(tc1,154,tp,tp,false,false,POS_FACEUP)
		Duel.SpecialSummonStep(tc2,154,tp,tp,false,false,POS_FACEUP)
		tc1:RegisterFlagEffect(56240989,RESET_EVENT+0x1fe0000,0,1,fid)
		tc2:RegisterFlagEffect(56240989,RESET_EVENT+0x1fe0000,0,1,fid)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc1:RegisterEffect(e1,true)
		local e2=e1:Clone()
		tc2:RegisterEffect(e2,true)
		local e3=Effect.CreateEffect(c)
		e3:SetType(EFFECT_TYPE_SINGLE)
		e3:SetCode(EFFECT_DISABLE_EFFECT)
		e3:SetReset(RESET_EVENT+0x1fe0000)
		tc1:RegisterEffect(e3,true)
		local e4=e3:Clone()
		tc2:RegisterEffect(e4,true)
		Duel.SpecialSummonComplete()
		sg:AddCard(tc2)
		sg:KeepAlive()
		local e5=Effect.CreateEffect(c)
		e5:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e5:SetCode(EVENT_PHASE+PHASE_END)
		e5:SetCountLimit(1)
		e5:SetLabel(fid)
		e5:SetLabelObject(sg)
		e5:SetCondition(c56240989.rmcon)
		e5:SetOperation(c56240989.rmop)
		Duel.RegisterEffect(e5,tp)
	end
end
function c56240989.rmfilter(c,fid)
	return c:GetFlagEffectLabel(56240989)==fid
end
function c56240989.rmcon(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	if not g:IsExists(c56240989.rmfilter,1,nil,e:GetLabel()) then
		g:DeleteGroup()
		e:Reset()
		return false
	else return true end
end
function c56240989.rmop(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	local tg=g:Filter(c56240989.rmfilter,nil,e:GetLabel())
	Duel.Remove(tg,POS_FACEUP,REASON_EFFECT)
end
