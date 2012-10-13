--起爆獣ヴァルカノン
function c10365322.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcFun2(c,aux.FilterBoolFunction(Card.IsRace,RACE_MACHINE),aux.FilterBoolFunction(Card.IsRace,RACE_PYRO),true)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(10365322,0))
	e2:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetCondition(c10365322.descon)
	e2:SetTarget(c10365322.destg)
	e2:SetOperation(c10365322.desop)
	c:RegisterEffect(e2)
end
function c10365322.descon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(e:GetHandler():GetSummonType(),SUMMON_TYPE_FUSION)==SUMMON_TYPE_FUSION
end
function c10365322.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and chkc:IsDestructable() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDestructable,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,0,LOCATION_MZONE,1,1,nil)
	g:AddCard(e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,0)
end
function c10365322.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local c=e:GetHandler()
	if not tc:IsRelateToEffect(e) and not c:IsRelateToEffect(e) then return end
	if not tc:IsRelateToEffect(e) then
		Duel.Destroy(c,REASON_EFFECT)
	elseif not c:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	else
		local dg=Group.FromCards(c,tc)
		if Duel.Destroy(dg,REASON_EFFECT)==2 and tc:IsLocation(LOCATION_GRAVE) and c:IsLocation(LOCATION_GRAVE) then
			Duel.BreakEffect()
			local d=tc:GetAttack()
			if d<0 then d=0 end
			Duel.Damage(1-tp,d,REASON_EFFECT)
		end
	end
end
