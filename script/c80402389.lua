--大樹海
function c80402389.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--trigger
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c80402389.check)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e3:SetDescription(aux.Stringid(80402389,0))
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_EVENT_PLAYER)
	e3:SetCode(80402389)
	e3:SetTarget(c80402389.target)
	e3:SetOperation(c80402389.operation)
	c:RegisterEffect(e3)
end
function c80402389.check(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=eg:GetFirst()
	while tc do
		if tc:IsPreviousLocation(LOCATION_MZONE) and tc:IsReason(REASON_DESTROY)
			and tc:IsRace(RACE_INSECT) and tc:GetLevel()~=0 and tc:IsPreviousPosition(POS_FACEUP) then
			Duel.RaiseSingleEvent(c,80402389,e,r,rp,tc:GetControler(),tc:GetLevel())
		end
		tc=eg:GetNext()
	end
end
function c80402389.filter(c,lv)
	return c:IsRace(RACE_INSECT) and c:GetLevel()==lv and c:IsAbleToHand()
end
function c80402389.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e)
		and Duel.IsExistingMatchingCard(c80402389.filter,tp,LOCATION_DECK,0,1,nil,ev) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c80402389.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c80402389.filter,tp,LOCATION_DECK,0,1,1,nil,ev)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
