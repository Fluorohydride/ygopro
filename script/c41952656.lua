--イマイルカ
function c41952656.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(41952656,0))
	e1:SetCategory(CATEGORY_DECKDES+CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c41952656.drcon)
	e1:SetTarget(c41952656.drtg)
	e1:SetOperation(c41952656.drop)
	c:RegisterEffect(e1)
end
function c41952656.drcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:IsReason(REASON_DESTROY) and not c:IsReason(REASON_RULE) and rp==1-tp and c:GetPreviousControler()==tp
end
function c41952656.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DECKDES,nil,0,tp,1)
end
function c41952656.drop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.DiscardDeck(tp,1,REASON_EFFECT)==1 then
		local g=Duel.GetOperatedGroup()
		if g:GetFirst():IsAttribute(ATTRIBUTE_WATER) then
			Duel.Draw(tp,1,REASON_EFFECT)
		end
	end
end
