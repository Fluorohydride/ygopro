--ゾルガ
function c16268841.initial_effect(c)
	--recover
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(16268841,0))
	e1:SetCategory(CATEGORY_RECOVER)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_RELEASE)
	e1:SetCondition(c16268841.reccon)
	e1:SetTarget(c16268841.rectg)
	e1:SetOperation(c16268841.recop)
	c:RegisterEffect(e1)
end
function c16268841.reccon(e,tp,eg,ep,ev,re,r,rp)
	e:SetLabel(e:GetHandler():GetPreviousControler())
	return e:GetHandler():IsReason(REASON_SUMMON)
end
function c16268841.rectg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckEvent(EVENT_SUMMON_SUCCESS) or Duel.CheckEvent(EVENT_MSET) end
	Duel.SetTargetPlayer(e:GetLabel())
	Duel.SetTargetParam(2000)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,e:GetLabel(),2000)
end
function c16268841.recop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Recover(p,d,REASON_EFFECT)
end
