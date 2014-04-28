--星邪の神喰
function c72710085.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(72710085,0))
	e2:SetCategory(CATEGORY_TOGRAVE)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetProperty(EFFECT_FLAG_DELAY)
	e2:SetCode(EVENT_REMOVE)
	e2:SetCountLimit(1,72710085)
	e2:SetCondition(c72710085.tgcon)
	e2:SetTarget(c72710085.tgtg)
	e2:SetOperation(c72710085.tgop)
	c:RegisterEffect(e2)
end
function c72710085.tgcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	return eg:GetCount()==1 and tc:IsPreviousLocation(LOCATION_GRAVE) and tc:IsControler(tp) and tc:IsType(TYPE_MONSTER)
end
function c72710085.filter(c,att)
	return not c:IsAttribute(att) and c:IsType(TYPE_MONSTER) and c:IsAbleToGrave()
end
function c72710085.tgtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e)
		and Duel.IsExistingMatchingCard(c72710085.filter,tp,LOCATION_DECK,0,1,nil,eg:GetFirst():GetAttribute()) end
	e:SetLabel(eg:GetFirst():GetAttribute())
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
end
function c72710085.tgop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c72710085.filter,tp,LOCATION_DECK,0,1,1,nil,e:GetLabel())
	if g:GetCount()>0 then
		Duel.SendtoGrave(g,REASON_EFFECT)
	end
end
