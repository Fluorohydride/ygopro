--悪魔竜ブラック・デーモンズ・ドラゴン
function c45349196.initial_effect(c)
	c:SetSPSummonOnce(45349196)
	--fusion material
	aux.AddFusionProcFun2(c,c45349196.mfilter1,c45349196.mfilter2,true)
	c:EnableReviveLimit()
	--aclimit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_ACTIVATE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(0,1)
	e1:SetCondition(c45349196.accon)
	e1:SetValue(c45349196.aclimit)
	c:RegisterEffect(e1)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_DAMAGE+CATEGORY_TODECK)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_PHASE+PHASE_BATTLE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1)
	e2:SetCondition(c45349196.damcon)
	e2:SetTarget(c45349196.damtg)
	e2:SetOperation(c45349196.damop)
	c:RegisterEffect(e2)
end
function c45349196.mfilter1(c)
	return c:IsSetCard(0x45) and c:IsType(TYPE_NORMAL) and c:GetLevel()==6
end
function c45349196.mfilter2(c)
	return c:IsSetCard(0x3b) and c:IsType(TYPE_NORMAL)
end
function c45349196.accon(e)
	return Duel.GetAttacker()==e:GetHandler() or Duel.GetAttackTarget()==e:GetHandler()
end
function c45349196.aclimit(e,re,tp)
	return not re:GetHandler():IsImmuneToEffect(e)
end
function c45349196.damcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:GetSummonType()==SUMMON_TYPE_FUSION and c:GetBattledGroupCount()>0
end
function c45349196.filter(c)
	return c:IsSetCard(0x3b) and c:IsType(TYPE_NORMAL) and c:IsAbleToDeck()
end
function c45349196.damtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c45349196.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c45349196.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g=Duel.SelectTarget(tp,c45349196.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	local atk=g:GetFirst():GetBaseAttack()
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,atk)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,1,0,0)
end
function c45349196.damop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and Duel.Damage(1-tp,tc:GetBaseAttack(),REASON_EFFECT)~=0 then
		Duel.BreakEffect()
		Duel.SendtoDeck(tc,nil,2,REASON_EFFECT)
	end
end
