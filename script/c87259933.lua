--宝玉の集結
function c87259933.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetHintTiming(TIMING_DESTROY)
	e1:SetCondition(c87259933.spcon1)
	e1:SetTarget(c87259933.sptg1)
	e1:SetOperation(c87259933.spop)
	c:RegisterEffect(e1)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(87259933,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_DESTROYED)
	e2:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetCondition(c87259933.spcon2)
	e2:SetCost(c87259933.spcost)
	e2:SetTarget(c87259933.sptg2)
	e2:SetOperation(c87259933.spop)
	c:RegisterEffect(e2)
	--tohand
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(87259933,1))
	e3:SetCategory(CATEGORY_TOHAND)
	e3:SetType(EFFECT_TYPE_QUICK_O)
	e3:SetCode(EVENT_FREE_CHAIN)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCost(c87259933.thcost)
	e3:SetTarget(c87259933.thtg)
	e3:SetOperation(c87259933.thop)
	c:RegisterEffect(e3)
end
function c87259933.cfilter(c,tp)
	return c:IsSetCard(0x1034) and c:IsReason(REASON_BATTLE+REASON_EFFECT)
		and c:IsPreviousPosition(POS_FACEUP) and c:IsPreviousLocation(LOCATION_MZONE) and c:GetPreviousControler()==tp
end
function c87259933.spcon1(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetCurrentPhase()~=PHASE_DAMAGE then return true end
	local res,teg,tep,tev,tre,tr,trp=Duel.CheckEvent(EVENT_DESTROYED,true)
	return res and teg:IsExists(c87259933.cfilter,1,nil,tp)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c87259933.filter,tp,LOCATION_DECK,0,1,nil,e,tp)
end
function c87259933.sptg1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local res,teg,tep,tev,tre,tr,trp=Duel.CheckEvent(EVENT_DESTROYED,true)
	if Duel.GetCurrentPhase()==PHASE_DAMAGE
		or (res and teg:IsExists(c87259933.cfilter,1,nil,tp)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c87259933.filter,tp,LOCATION_DECK,0,1,nil,e,tp)
		and Duel.SelectYesNo(tp,aux.Stringid(87259933,2))) then
		e:SetCategory(CATEGORY_SPECIAL_SUMMON)
		Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
		e:GetHandler():RegisterFlagEffect(87259933,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
		e:GetHandler():RegisterFlagEffect(0,RESET_CHAIN,EFFECT_FLAG_CLIENT_HINT,1,0,aux.Stringid(87259933,3))
	else
		e:SetCategory(0)
	end
end
function c87259933.spcon2(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c87259933.cfilter,1,nil,tp)
end
function c87259933.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(87259933)==0 end
	e:GetHandler():RegisterFlagEffect(87259933,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c87259933.filter(c,e,tp)
	return c:IsSetCard(0x1034) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c87259933.sptg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c87259933.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c87259933.spop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():GetFlagEffect(87259933)==0 then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c87259933.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
function c87259933.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c87259933.thfilter1(c,rc)
	return c:IsFaceup() and c:IsSetCard(0x1034) and c:IsAbleToHand()
		and Duel.IsExistingTarget(c87259933.thfilter2,0,LOCATION_ONFIELD,LOCATION_ONFIELD,1,c,rc)
end
function c87259933.thfilter2(c,rc)
	return c~=rc and c:IsAbleToHand()
end
function c87259933.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return not e:GetHandler():IsStatus(STATUS_CHAINING)
		and Duel.IsExistingTarget(c87259933.thfilter1,tp,LOCATION_ONFIELD,0,1,nil,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g1=Duel.SelectTarget(tp,c87259933.thfilter1,tp,LOCATION_ONFIELD,0,1,1,nil,e:GetHandler())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g2=Duel.SelectTarget(tp,c87259933.thfilter2,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,g1:GetFirst(),e:GetHandler())
	g1:Merge(g2)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g1,2,0,0)
end
function c87259933.thop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
	end
end
