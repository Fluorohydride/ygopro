--英雄変化－リフレクター・レイ
function c74095602.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c74095602.condition)
	e1:SetTarget(c74095602.target)
	e1:SetOperation(c74095602.activate)
	c:RegisterEffect(e1)
end
function c74095602.condition(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=eg:GetFirst()
	return tc:GetPreviousControler()==tp and tc:IsType(TYPE_FUSION) and tc:IsSetCard(0x3008)
		and tc:IsLocation(LOCATION_GRAVE) and tc:IsReason(REASON_BATTLE) 
end
function c74095602.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local tc=eg:GetFirst()
	local lv=tc:GetLevel()
	e:SetLabel(lv)
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(lv*300)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,lv*300)
end
function c74095602.activate(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
