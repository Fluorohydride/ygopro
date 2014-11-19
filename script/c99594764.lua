--ドラグニティ－コルセスカ
function c99594764.initial_effect(c)
	--search
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(99594764,0))
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLE_DESTROYING)
	e1:SetRange(LOCATION_SZONE)
	e1:SetCondition(c99594764.thcon)
	e1:SetTarget(c99594764.thtg)
	e1:SetOperation(c99594764.thop)
	c:RegisterEffect(e1)
end
function c99594764.thcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:GetCount()==1 and eg:GetFirst()==e:GetHandler():GetEquipTarget()
end
function c99594764.filter(c,race,att)
	return c:IsRace(race) and c:IsAttribute(att) and c:GetLevel()<=4 and c:IsAbleToHand()
end
function c99594764.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	local eqc=e:GetHandler():GetEquipTarget()
	if chk==0 then return Duel.IsExistingMatchingCard(c99594764.filter,tp,LOCATION_DECK,0,1,nil,eqc:GetRace(),eqc:GetAttribute()) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c99594764.thop(e,tp,eg,ep,ev,re,r,rp)
	local eqc=e:GetHandler():GetEquipTarget()
	if not eqc then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c99594764.filter,tp,LOCATION_DECK,0,1,1,nil,eqc:GetRace(),eqc:GetAttribute())
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
