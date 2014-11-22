--CX 機装魔人エンジェネラル
function c41309158.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,4,3)
	c:EnableReviveLimit()
	--
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(41309158,0))
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c41309158.poscon)
	e1:SetTarget(c41309158.postg)
	e1:SetOperation(c41309158.posop)
	c:RegisterEffect(e1)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(41309158,1))
	e2:SetCategory(CATEGORY_DAMAGE)
	e2:SetCode(EVENT_BATTLE_DAMAGE)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetCondition(c41309158.damcon)
	e2:SetCost(c41309158.damcost)
	e2:SetTarget(c41309158.damtg)
	e2:SetOperation(c41309158.damop)
	c:RegisterEffect(e2)
end
function c41309158.poscon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_XYZ
end
function c41309158.postg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsDefencePos() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDefencePos,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DEFENCE)
	Duel.SelectTarget(tp,Card.IsDefencePos,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
end
function c41309158.posop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsDefencePos() and tc:IsRelateToEffect(e) then
		Duel.ChangePosition(tc,POS_FACEUP_ATTACK)
	end
end
function c41309158.damcon(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp and e:GetHandler():GetOverlayGroup():IsExists(Card.IsCode,1,nil,15914410)
end
function c41309158.damcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c41309158.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(1000)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,1000)
end
function c41309158.damop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
