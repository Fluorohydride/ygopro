--昇華する紋章
function c59048135.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--search
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(59048135,0))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetCost(c59048135.thcost)
	e2:SetTarget(c59048135.thtg)
	e2:SetOperation(c59048135.thop)
	c:RegisterEffect(e2)
	--cannot be target
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e3:SetTarget(c59048135.etarget)
	e3:SetValue(c59048135.evalue)
	c:RegisterEffect(e3)
	Duel.AddCustomActivityCounter(59048135,ACTIVITY_SUMMON,c59048135.counterfilter)
	Duel.AddCustomActivityCounter(59048135,ACTIVITY_SPSUMMON,c59048135.counterfilter)
end
function c59048135.counterfilter(c)
	return c:IsRace(RACE_PSYCHO) and c:IsType(TYPE_XYZ) or c:IsSetCard(0x76)
end
function c59048135.etarget(e,c)
	return c:IsRace(RACE_PSYCHO) and c:IsType(TYPE_XYZ)
end
function c59048135.evalue(e,re,rp)
	return re:IsActiveType(TYPE_SPELL+TYPE_TRAP) and aux.tgval(e,re,rp)
end
function c59048135.cfilter(c)
	return c:IsSetCard(0x76) and c:IsType(TYPE_MONSTER) and c:IsDiscardable()
end
function c59048135.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetCustomActivityCount(59048135,tp,ACTIVITY_SUMMON)==0
		and Duel.GetCustomActivityCount(59048135,tp,ACTIVITY_SPSUMMON)==0
		and Duel.IsExistingMatchingCard(c59048135.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c59048135.cfilter,1,1,REASON_COST+REASON_DISCARD,nil)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetTargetRange(1,0)
	e1:SetTarget(c59048135.splimit)
	Duel.RegisterEffect(e1,tp)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_CANNOT_SUMMON)
	Duel.RegisterEffect(e2,tp)
end
function c59048135.splimit(e,c)
	return not (c:IsRace(RACE_PSYCHO) and c:IsType(TYPE_XYZ)) and not c:IsSetCard(0x76)
end
function c59048135.filter(c)
	return c:IsSetCard(0x92) and c:IsType(TYPE_SPELL+TYPE_TRAP) and not c:IsCode(59048135) and c:IsAbleToHand()
end
function c59048135.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c59048135.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c59048135.thop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c59048135.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
