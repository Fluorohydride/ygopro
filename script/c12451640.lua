--フロント・オブザーバー
function c12451640.initial_effect(c)
	--
	e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetOperation(c12451640.regop)
	c:RegisterEffect(e1)
end
function c12451640.regop(e,tp,eg,ep,ev,re,r,rp)
	c=e:GetHandler()
	c:RegisterFlagEffect(12451640,RESET_EVENT+0x1fe0000,0,1,Duel.GetTurnCount())
	--to hand
	e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(12451640,0))
	e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetRange(LOCATION_MZONE)
	e1:SetLabel(Duel.GetTurnCount())
	e1:SetCountLimit(1)
	e1:SetTarget(c12451640.thtg)
	e1:SetOperation(c12451640.thop)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	c:RegisterEffect(e1)
	--to hand
	e2=Effect.Clone(e1)
	e2:SetDescription(aux.Stringid(12451640,1))
	e2:SetCondition(c12451640.thcon)
	e2:SetCost(c12451640.thcost)
	e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END+RESET_SELF_TURN,2)
	c:RegisterEffect(e2)
end
function c12451640.filter(c,d)
	local res=c:IsAttribute(ATTRIBUTE_EARTH) and c:IsAbleToHand()
	if d==0 then
		return res and c:IsType(TYPE_PENDULUM)
	else
		return res
	end
end
function c12451640.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	local d=Duel.GetTurnCount()-e:GetLabel()
	if chk==0 then return Duel.IsExistingMatchingCard(c12451640.filter,tp,LOCATION_DECK,0,1,nil,d) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c12451640.thop(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.GetTurnCount()-e:GetLabel()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c12451640.filter,tp,LOCATION_DECK,0,1,1,nil,d)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
function c12451640.thcon(e,tp,eg,ep,ev,re,r,rp)
	return  Duel.GetTurnPlayer()==tp and Duel.GetTurnCount()~=e:GetLabel() and e:GetHandler():GetFlagEffectLabel(12451640)==e:GetLabel()
end
function c12451640.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
