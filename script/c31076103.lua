--第一の棺
function c31076103.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(31076103,0))
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_PHASE+PHASE_END)
	e2:SetCountLimit(1)
	e2:SetCondition(c31076103.condition)
	e2:SetOperation(c31076103.operation)
	c:RegisterEffect(e2)
	--spsummon
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(31076103,1))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_PHASE+PHASE_END)
	e3:SetCountLimit(1)
	e3:SetCondition(c31076103.condition)
	e3:SetCost(c31076103.spcost)
	e3:SetTarget(c31076103.sptg)
	e3:SetOperation(c31076103.spop)
	c:RegisterEffect(e3)
	--tograve
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e4:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCode(EVENT_LEAVE_FIELD)
	e4:SetCondition(c31076103.tgcon)
	e4:SetOperation(c31076103.tgop)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e5:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e5:SetCode(EVENT_LEAVE_FIELD)
	e5:SetOperation(c31076103.tgop)
	c:RegisterEffect(e5)
end
function c31076103.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp
end
function c31076103.cfilter1(c,code)
	return c:IsFaceup() and c:IsCode(code)
end
function c31076103.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_SZONE)<=0 then return end
	if not Duel.IsExistingMatchingCard(c31076103.cfilter1,tp,LOCATION_SZONE,0,1,nil,4081094) then
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(31076103,2))
		local g=Duel.SelectMatchingCard(tp,Card.IsCode,tp,LOCATION_DECK+LOCATION_HAND,0,1,1,nil,4081094)
		if g:GetCount()>0 then
			Duel.MoveToField(g:GetFirst(),tp,tp,LOCATION_SZONE,POS_FACEUP,true)
		end
	elseif not Duel.IsExistingMatchingCard(c31076103.cfilter1,tp,LOCATION_SZONE,0,1,nil,78697395) then
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(31076103,2))
		local g=Duel.SelectMatchingCard(tp,Card.IsCode,tp,LOCATION_DECK+LOCATION_HAND,0,1,1,nil,78697395)
		if g:GetCount()>0 then
			Duel.MoveToField(g:GetFirst(),tp,tp,LOCATION_SZONE,POS_FACEUP,true)
		end
	end
end
function c31076103.cfilter2(c,code)
	return c:IsFaceup() and c:IsCode(code) and c:IsAbleToGraveAsCost()
end
function c31076103.cfilter3(c)
	local code=c:GetCode()
	return c:IsFaceup() and (code==31076103 or code==4081094 or code==78697395) and c:IsAbleToGraveAsCost()
end
function c31076103.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost()
		and Duel.IsExistingMatchingCard(c31076103.cfilter2,tp,LOCATION_SZONE,0,1,nil,4081094)
		and Duel.IsExistingMatchingCard(c31076103.cfilter2,tp,LOCATION_SZONE,0,1,nil,78697395) end
	local g=Duel.GetMatchingGroup(c31076103.cfilter3,tp,LOCATION_SZONE,0,nil)
	Duel.SendtoGrave(g,REASON_COST)
end
function c31076103.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND+LOCATION_DECK)
end
function c31076103.spfilter(c,e,tp)
	return c:IsCode(25343280) and c:IsCanBeSpecialSummoned(e,0,tp,true,false)
end
function c31076103.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c31076103.spfilter,tp,LOCATION_DECK+LOCATION_HAND,0,1,1,nil,e,tp)
	if g:GetCount()>0 and Duel.SpecialSummon(g,0,tp,tp,true,false,POS_FACEUP)>0 then
		g:GetFirst():CompleteProcedure()
	end
end
function c31076103.cfilter4(c)
	local code=c:GetCode()
	return code==31076103 or code==4081094 or code==78697395
end
function c31076103.cfilter5(c)
	local code=c:GetCode()
	return c:IsFaceup() and (code==31076103 or code==4081094 or code==78697395)
end
function c31076103.tgcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c31076103.cfilter4,1,nil)
end
function c31076103.tgop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c31076103.cfilter5,tp,LOCATION_SZONE,0,nil)
	Duel.SendtoGrave(g,REASON_EFFECT)
end
