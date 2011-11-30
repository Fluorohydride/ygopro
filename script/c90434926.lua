--漆黒のトバリ
function c90434926.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--draw
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(90434926,0))
	e2:SetRange(LOCATION_SZONE)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_DRAW)
	e2:SetCondition(c90434926.drcon)
	e2:SetCost(c90434926.drcost)
	e2:SetTarget(c90434926.drtg)
	e2:SetOperation(c90434926.drop)
	c:RegisterEffect(e2)
end
function c90434926.drcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp and Duel.GetCurrentPhase()==PHASE_DRAW
end
function c90434926.filter(c)
	return c:IsAttribute(ATTRIBUTE_DARK) and not c:IsPublic()
end
function c90434926.drcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return ep==tp and eg:IsExists(c90434926.filter,1,nil) end
	local g=eg:Filter(c90434926.filter,nil)
	if g:GetCount()==1 then
		Duel.ConfirmCards(1-tp,g)
		Duel.ShuffleHand(tp)
		e:SetLabelObject(g:GetFirst())
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
		local sg=g:Select(tp,1,1,nil)
		Duel.ConfirmCards(1-tp,sg)
		Duel.ShuffleHand(tp)
		e:SetLabelObject(sg:GetFirst())
	end
end
function c90434926.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local tc=e:GetLabelObject()
	tc:CreateEffectRelation(e)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,tc,1,0,0)
end
function c90434926.drop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	if not e:GetHandler():IsRelateToEffect(e) or not tc:IsRelateToEffect(e) then return end
	Duel.SendtoGrave(tc,REASON_EFFECT)
	if tc:IsLocation(LOCATION_GRAVE) and Duel.IsPlayerCanDraw(tp,1) and Duel.SelectYesNo(tp,aux.Stringid(90434926,1)) then
		Duel.BreakEffect()
		Duel.Draw(tp,1,REASON_EFFECT)
	end
end
