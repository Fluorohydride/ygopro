--エクシーズ・アヴェンジャー
function c86062400.initial_effect(c)
	--immune
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_IMMUNE_EFFECT)
	e1:SetValue(c86062400.efilter)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(86062400,0))
	e2:SetCategory(CATEGORY_TOGRAVE)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_BATTLE_DESTROYED)
	e2:SetCondition(c86062400.tgcon)
	e2:SetTarget(c86062400.tgtg)
	e2:SetOperation(c86062400.tgop)
	c:RegisterEffect(e2)
end
function c86062400.efilter(e,te)
	return te:IsActiveType(TYPE_XYZ)
end
function c86062400.tgcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	return c:IsLocation(LOCATION_GRAVE) and c:IsReason(REASON_BATTLE)
		and bc:IsFaceup() and bc:IsRelateToBattle() and bc:IsType(TYPE_XYZ)
end
function c86062400.tgtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local rk=e:GetHandler():GetBattleTarget():GetRank()
	if rk<5 then
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,1-tp,LOCATION_EXTRA)
	else
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,rk,1-tp,LOCATION_EXTRA)
	end
end
function c86062400.tgop(e,tp,eg,ep,ev,re,r,rp)
	local bc=e:GetHandler():GetBattleTarget()
	if bc:IsFaceup() and bc:IsRelateToBattle() then
		local rk=bc:GetRank()
		local g=nil
		local tg=Duel.GetFieldGroup(tp,0,LOCATION_EXTRA)
		if rk<4 then
			Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TOGRAVE)
			g=tg:Select(1-tp,1,1,nil)
		elseif rk==4 then
			Duel.ConfirmCards(tp,tg)
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
			g=tg:Select(tp,1,1,nil)
		else
			Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TOGRAVE)
			g=tg:Select(1-tp,rk,rk,nil)
		end
		if g:GetCount()>0 then
			Duel.SendtoGrave(g,REASON_EFFECT)
		end
	end
end
