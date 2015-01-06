--チェイス・スカッド
function c27870033.initial_effect(c)
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(27870033,0))
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetType(EFFECT_TYPE_TRIGGER_F+EFFECT_TYPE_FIELD)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c27870033.damcon)
	e1:SetTarget(c27870033.damtg)
	e1:SetOperation(c27870033.damop)
	c:RegisterEffect(e1)
end
function c27870033.cfilter(c)
	return c:IsPreviousPosition(POS_DEFENCE) and c:IsLocation(LOCATION_GRAVE)
		and c:IsReason(REASON_BATTLE) and c:IsType(TYPE_MONSTER)
end
function c27870033.damcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c27870033.cfilter,1,nil)
end
function c27870033.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) and e:GetHandler():IsFaceup() end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(500)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,500)
end
function c27870033.damop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
