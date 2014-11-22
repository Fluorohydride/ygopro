--円卓の聖騎士
function c55742055.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(55742055,0))
	e2:SetCategory(CATEGORY_TOGRAVE)
	e2:SetType(EFFECT_TYPE_TRIGGER_O+EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_PHASE+PHASE_END)
	e2:SetLabel(3)
	e2:SetCountLimit(1)
	e2:SetCondition(c55742055.effcon)
	e2:SetTarget(c55742055.target1)
	e2:SetOperation(c55742055.operation1)
	c:RegisterEffect(e2)
	--Special Summon
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(55742055,1))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_TRIGGER_O+EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_PHASE+PHASE_END)
	e3:SetLabel(6)
	e3:SetCountLimit(1)
	e3:SetCondition(c55742055.effcon)
	e3:SetTarget(c55742055.target2)
	e3:SetOperation(c55742055.operation2)
	c:RegisterEffect(e3)
	--tohand
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(55742055,2))
	e4:SetCategory(CATEGORY_TOHAND)
	e4:SetType(EFFECT_TYPE_TRIGGER_O+EFFECT_TYPE_FIELD)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCode(EVENT_PHASE+PHASE_END)
	e4:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e4:SetLabel(9)
	e4:SetCountLimit(1)
	e4:SetCondition(c55742055.effcon)
	e4:SetTarget(c55742055.target3)
	e4:SetOperation(c55742055.operation3)
	c:RegisterEffect(e4)
	-- draw
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(55742055,3))
	e5:SetCategory(CATEGORY_DRAW)
	e5:SetType(EFFECT_TYPE_TRIGGER_O+EFFECT_TYPE_FIELD)
	e5:SetRange(LOCATION_SZONE)
	e5:SetCode(EVENT_PHASE+PHASE_END)
	e5:SetCountLimit(1)
	e5:SetCondition(c55742055.condition4)
	e5:SetTarget(c55742055.target4)
	e5:SetOperation(c55742055.operation4)
	c:RegisterEffect(e5)
end
function c55742055.confilter(c)
	return c:IsFaceup() and c:IsSetCard(0x107a)
end
function c55742055.effcon(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetTurnPlayer()~=tp then return false end
	local g=Duel.GetMatchingGroup(c55742055.confilter,tp,LOCATION_GRAVE+LOCATION_ONFIELD,0,nil)
	return g:GetClassCount(Card.GetCode)>=e:GetLabel()
end
function c55742055.filter1(c)
	return c:IsSetCard(0x107a) and c:IsAbleToGrave()
end
function c55742055.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c55742055.filter1,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,0,1,tp,LOCATION_DECK)
end
function c55742055.operation1(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c55742055.filter1,tp,LOCATION_DECK,0,1,1,nil)
	Duel.SendtoGrave(g,REASON_EFFECT)
end
function c55742055.filter2(c,e,tp)
	return  c:IsSetCard(0x107a) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c55742055.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c55742055.filter2,tp,LOCATION_HAND,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c55742055.eqfilter(c,tc,tp)
	return c:IsType(TYPE_EQUIP) and c:IsSetCard(0x207a) and c:CheckEquipTarget(tc) and c:CheckUniqueOnField(tp)
end
function c55742055.operation2(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c55742055.filter2,tp,LOCATION_HAND,0,1,1,nil,e,tp)
	local tc=g:GetFirst()
	if Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)==0 then return end
	local tg=Duel.GetMatchingGroup(c55742055.eqfilter,tp,LOCATION_HAND,0,nil,tc,tp)
	if tg:GetCount()>0 and Duel.GetLocationCount(tp,LOCATION_SZONE)>0 and Duel.SelectYesNo(tp,aux.Stringid(55742055,4)) then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
		local sg=tg:Select(tp,1,1,nil)
		Duel.Equip(tp,sg:GetFirst(),tc,true)
	end
end
function c55742055.thfilter(c)
	return c:IsSetCard(0x107a) and c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c55742055.target3(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c55742055.thfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c55742055.thfilter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c55742055.thfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c55742055.operation3(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
end
function c55742055.condition4(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetTurnPlayer()~=tp then return false end
	local g=Duel.GetMatchingGroup(c55742055.confilter,tp,LOCATION_GRAVE+LOCATION_ONFIELD,0,nil)
	return g:GetClassCount(Card.GetCode)==12
end
function c55742055.target4(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) end
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c55742055.operation4(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Draw(tp,1,REASON_EFFECT)
end
