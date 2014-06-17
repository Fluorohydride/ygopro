--ナチュルの神星樹
function c80200076.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetCountLimit(1,EFFECT_COUNT_CODE_SINGLE)
	e1:SetTarget(c80200076.target1)
	e1:SetOperation(c80200076.operation)
	c:RegisterEffect(e1)
	--instant
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(80200076,0))
	e2:SetCategory(CATEGORY_TOGRAVE+CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetHintTiming(0,TIMING_END_PHASE)
	e2:SetCountLimit(1,EFFECT_COUNT_CODE_SINGLE)
	e2:SetCost(c80200076.cost)
	e2:SetTarget(c80200076.target2)
	e2:SetOperation(c80200076.operation)
	c:RegisterEffect(e2)
	--search
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(80200076,1))
	e3:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e3:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e3:SetType(EFFECT_TYPE_TRIGGER_O+EFFECT_TYPE_SINGLE)
	e3:SetCode(EVENT_TO_GRAVE)
	e3:SetTarget(c80200076.tg)
	e3:SetOperation(c80200076.op)
	c:RegisterEffect(e3)	
end
function c80200076.spfilter(c,e,tp,race)
	return c:IsAttribute(ATTRIBUTE_EARTH) and c:IsRace(race) and c:IsLevelBelow(4) and c:IsCanBeSpecialSummoned(e,170,tp,false,false)
end
function c80200076.costfilter1(c,e,tp)
	return c:IsAttribute(ATTRIBUTE_EARTH) and c:IsRace(RACE_INSECT) and c:IsReleasable() and Duel.IsExistingMatchingCard(c80200076.spfilter,tp,LOCATION_DECK,0,1,nil,e,tp,RACE_PLANT)
end
function c80200076.costfilter2(c,e,tp)
	return c:IsAttribute(ATTRIBUTE_EARTH) and c:IsRace(RACE_PLANT) and c:IsReleasable() and Duel.IsExistingMatchingCard(c80200076.spfilter,tp,LOCATION_DECK,0,1,nil,e,tp,RACE_INSECT)
end
function c80200076.cost(e,tp,eg,ep,ev,re,r,rp,chkc)
	if chkc==0 then return Duel.GetFlagEffect(tp,80200076)==0 and 
	(Duel.CheckReleaseGroup(tp,c80200076.costfilter1,1,nil,e,tp) or Duel.CheckReleaseGroup(tp,c80200076.costfilter2,1,nil,e,tp)) end
	local sg1=Duel.GetMatchingGroup(c80200076.costfilter1,tp,LOCATION_MZONE,0,nil,e,tp)
	local sg2=Duel.GetMatchingGroup(c80200076.costfilter2,tp,LOCATION_MZONE,0,nil,e,tp)
	sg1:Merge(sg2)
	local rg=sg1:Select(tp,1,1,nil)
	Duel.Release(rg,REASON_COST)
	if rg:GetFirst():GetRace()==RACE_INSECT then
		e:SetLabel(RACE_PLANT)
	else 
		e:SetLabel(RACE_INSECT)
	end
	Duel.RegisterFlagEffect(tp,80200076,RESET_PHASE+PHASE_END,0,1)
end
function c80200076.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local sg1=Duel.GetMatchingGroup(c80200076.costfilter1,tp,LOCATION_MZONE,0,nil,e,tp)
	local sg2=Duel.GetMatchingGroup(c80200076.costfilter2,tp,LOCATION_MZONE,0,nil,e,tp)
	sg1:Merge(sg2)
	e:SetLabel(0)
	if Duel.GetFlagEffect(tp,80200076)==0 and sg1:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(80200076,2)) then
		local rg=sg1:Select(tp,1,1,nil)
		Duel.Release(rg,REASON_COST)
		if rg:GetFirst():GetRace()==RACE_INSECT then
			e:SetLabel(RACE_PLANT)
		else 
			e:SetLabel(RACE_INSECT)
		end
		Duel.RegisterFlagEffect(tp,80200076,RESET_PHASE+PHASE_END,0,1)
	end
end
function c80200076.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 or not e:GetHandler():IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c80200076.spfilter,tp,LOCATION_DECK,0,1,1,nil,e,tp,e:GetLabel())
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
function c80200076.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1 
	end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c80200076.filter1(c)
	return c:IsSetCard(0x2a) and c:IsAbleToHand() and not c:IsCode(80200076)
end
function c80200076.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c80200076.filter1,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c80200076.op(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c80200076.filter1,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end